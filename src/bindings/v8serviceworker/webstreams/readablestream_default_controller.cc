/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/queue_chunk.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_default_controller.h>
#include <bindings/v8serviceworker/webstreams/stream_promise.h>
#include <bindings/v8serviceworker/webstreams/underlying_source.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_value.h>

static void readablestream_controller_js_constructor(
    const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void readablestream_controller_js_desiredSize_getter(
    const v8::FunctionCallbackInfo<v8::Value> &args) {
  using v8serviceworker::ReadableStream;
  using v8serviceworker::ReadableStreamState_Closed;
  using v8serviceworker::ReadableStreamState_Errored;
  auto rs = v8wrap::get_ptr<ReadableStream>(args.Holder());

  // If state is "errored", return null.
  if (rs->state_ == ReadableStreamState_Errored) {
    args.GetReturnValue().SetNull();
    return;
  }
  // If state is "closed", return 0
  if (rs->state_ == ReadableStreamState_Closed) {
    args.GetReturnValue().Set(0);
    return;
  }
  // If state is "readable", return desiredSize
  args.GetReturnValue().Set(int32_t(rs->getDesiredSize()));
}

static void readablestream_controller_js_close(const v8::FunctionCallbackInfo<v8::Value> &args) {
  using v8serviceworker::ReadableStreamDefaultController;

  auto isolate = args.GetIsolate();
  auto controller = v8wrap::get_ptr<ReadableStreamDefaultController>(args.Holder());

  // https://streams.spec.whatwg.org/#readable-stream-default-controller-close
  // 1. Let stream be controller.[[controlledReadableStream]].
  auto stream = controller->stream_;
  // 2. Assert: ! ReadableStreamDefaultControllerCanCloseOrEnqueue(controller)
  //    is true.
  if (readableStreamDefaultControllerCanCloseOrEnqueue(controller)) {
    v8wrap::throw_type_error(isolate,
                             "can not close a stream that has been requested to be closed");
    return;
  }
  // 3. Set controller.[[closeRequested]] to true.
  controller->is_close_requested_ = true;

  // 4. If controller.[[queue]] is empty,
  if (controller->isQueueEmpty()) {
    // a. Perform ! ReadableStreamDefaultControllerClearAlgorithms(controller).
    controller->clearAlgorithms();

    // b. Perform ! ReadableStreamClose(stream).
    stream->setClose(isolate);
  }
}

static void readablestream_controller_js_enqueue(const v8::FunctionCallbackInfo<v8::Value> &args) {
  using v8serviceworker::ReadableStreamDefaultController;
  auto controller = v8wrap::get_ptr<ReadableStreamDefaultController>(args.Holder());

  // https://streams.spec.whatwg.org/#readable-stream-default-controller-enqueue
  // 1. Let stream be controller.[[controlledReadableStream]].
  const auto stream = controller->stream_;

  // 2. Assert: ! ReadableStreamDefaultControllerCanCloseOrEnqueue(controller)
  //    is true.
  assert(readableStreamDefaultControllerCanCloseOrEnqueue(controller));

  // 3. If ! IsReadableStreamLocked(stream) is true and !
  //    ReadableStreamGetNumReadRequests(stream) > 0, perform !
  //    ReadableStreamFulfillReadRequest(stream, chunk, false).
  if (stream->isLocked() && stream->getNumReadRequests() > 0) {
    // TODO(fxh): readableStreamFulfillReadRequest(stream, args[0], false);
  } else {
    // 4. Otherwise,
    //   a. Let result be the result of performing controller.
    //      [[strategySizeAlgorithm]], passing in chunk, and interpreting the
    //      result as an ECMAScript completion value.
    //   b. If result is an abrupt completion,
    auto sizeValue = stream->callSizeAlgorithm(args.GetIsolate()->GetCurrentContext());
    auto chunk = args[0];
    //  c. Let chunkSize be result.[[Value]].
    //  d. Let enqueueResult be EnqueueValueWithSize(controller, chunk,
    //     chunkSize).
    readableStreamDefaultControllerEnqueueValueWithSize(args, controller, chunk, sizeValue);
  }

  // 5. Perform ! ReadableStreamDefaultControllerCallPullIfNeeded(controller).
  readableStreamDefaultControllerCallPullIfNeeded(args.GetIsolate(), controller);
}

static void readablestream_controller_js_error(const v8::FunctionCallbackInfo<v8::Value> &args) {
  using v8serviceworker::ReadableStreamDefaultController;

  auto isolate = args.GetIsolate();
  auto controller = v8wrap::get_ptr<ReadableStreamDefaultController>(args.Holder());

  // https://streams.spec.whatwg.org/#readable-stream-default-controller-error
  // 1. Let stream be controller.[[controlledReadableStream]].
  auto stream = controller->stream_;
  // 2. If stream.[[state]] is not "readable", return.
  using v8serviceworker::ReadableStreamState_Readable;
  if (stream->state_ != ReadableStreamState_Readable) {
    return;
  }

  // 3. Perform ! ResetQueue(controller).
  controller->resetQueue();
  // 4. Perform ! ReadableStreamDefaultControllerClearAlgorithms(controller).
  controller->clearAlgorithms();
  // 5. Perform ! ReadableStreamError(stream, e).
  if (args.Length() > 0) {
    stream->setError(isolate, args[0]);
  } else {
    stream->setError(isolate, v8::Local<v8::Value>());
  }
}

namespace v8serviceworker {

// --- ReadableStreamDefaultController method ---

void ReadableStreamDefaultController::enqueue(v8::Isolate *isolate, v8::Local<v8::Value> value,
                                              int64_t size) {
  auto chunk = v8rt::allocObject<QueueChunk>(isolate);
  hlogd("ReadableStreamDefaultController::enqueue, chunk:%p", chunk);
  chunk->setValue(isolate, value, size);
  queue_.push(chunk);
}

void ReadableStreamDefaultController::resetQueue() {
  std::queue<QueueChunk *> empty;
  queue_.swap(empty);
  queue_total_size_ = 0;
}

void ReadableStreamDefaultController::clearAlgorithms() {}

QueueChunk *ReadableStreamDefaultController::dequeue() {
  auto chunk = queue_.front();
  queue_.pop();
  queue_total_size_ -= chunk->getSize();
  if (queue_total_size_ < 0) {
    queue_total_size_ = 0;
  }
  return chunk;
}

v8::Local<v8::Promise> ReadableStreamDefaultController::pullSteps(v8::Local<v8::Context> context) {
  auto isolate = context->GetIsolate();
  // https://streams.spec.whatwg.org/#rs-default-controller-private-pull
  // 1. Let stream be this.[[stream]].
  // 2. If this.[[queue]] is not empty,
  if (!queue_.empty()) {
    // a. Let chunk be ! DequeueValue(this).
    auto chunk = dequeue();
    hlogd("ReadableStreamDefaultController::pullSteps, dequeue, chunk: %p", chunk);
    // b. If this.[[closeRequested]] is true and this.[[queue]] is empty,
    if (queue_.empty() && is_close_requested_) {
      clearAlgorithms();
      stream_->setClose(isolate);
    } else {
      // c. Otherwise, perform !
      //    ReadableStreamDefaultControllerCallPullIfNeeded(this).
      readableStreamDefaultControllerCallPullIfNeeded(isolate, this);
      // d. Return a promise resolved with !
      //    ReadableStreamCreateReadResult(chunk, false,
      //    stream.[[reader]].[[forAuthorCode]]).
      auto result = ReadableStream::CreateReadResult(isolate, chunk->getValue(isolate), false);
      return StreamPromise::CreateResolved(isolate, result)->getPromise(isolate);
    }
  }
}

v8::Local<v8::Promise> ReadableStreamDefaultController::cancelSteps(v8::Local<v8::Context> context,
                                                                    v8::Local<v8::Value> reason) {
  // https://streams.spec.whatwg.org/#rs-default-controller-private-cancel
  // 1. Perform ! ResetQueue(this).
  resetQueue();
  // 2. Let result be the result of performing this.[[cancelAlgorithm]], passing
  //    reason.
  auto isolate = context->GetIsolate();
  auto controller_obj = js_object_.Get(isolate);
  auto result = source_->call_cancel(controller_obj);
  // 3. Perform ! ReadableStreamDefaultControllerClearAlgorithms(this).
  clearAlgorithms();
  // 4. Return result.
  return result;
}

// --- ReadableStreamDefaultController functions ---

v8::Local<v8::FunctionTemplate> create_readablestream_controller_template(
    v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder rsBuilder(isolateData->get_isolate(),
                                 CLASS_READABLE_STREAM_DEFAULT_CONTROLLER);
  rsBuilder.setConstructor(readablestream_controller_js_constructor);
  rsBuilder.setMethod("close", readablestream_controller_js_close);
  rsBuilder.setMethod("enqueue", readablestream_controller_js_enqueue);
  rsBuilder.setMethod("error", readablestream_controller_js_error);
  rsBuilder.setAccessorProperty("desiredSize", readablestream_controller_js_desiredSize_getter,
                                nullptr);
  auto rsTemplate = rsBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_READABLE_STREAM_DEFAULT_CONTROLLER, rsTemplate);

  return rsTemplate;
}

static void controller_pullAlgorithm_resolved(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto controller = v8wrap::get_ptr<ReadableStreamDefaultController>(args);
  // https://streams.spec.whatwg.org/#readable-stream-default-controller-call-pull-if-needed
  // 7. Upon fulfillment of pullPromise,
  //   a. Set controller.[[pulling]] to false.
  controller->pulling_ = false;

  //   b. If controller.[[pullAgain]] is true,
  if (controller->pull_again_) {
    //  i. Set controller.[[pullAgain]] to false.
    controller->pull_again_ = false;

    //  ii. Perform ! ReadableStreamDefaultControllerCallPullIfNeeded(
    //      controller).
    readableStreamDefaultControllerCallPullIfNeeded(args.GetIsolate(), controller);
  }
}
static void controller_pullAlgorithm_rejected(const v8::FunctionCallbackInfo<v8::Value> &args) {
  printf("controller_pullAlgorithm_rejected\n");
}

bool readableStreamDefaultControllerCanCloseOrEnqueue(ReadableStreamDefaultController *controller) {
  // https://streams.spec.whatwg.org/#readable-stream-default-controller-can-close-or-enqueue
  // 1. Let state be controller.[[controlledReadableStream]].[[state]].
  auto state = controller->stream_->state_;
  // 2. If controller.[[closeRequested]] is false and state is "readable",
  //    return true.
  // 3. Otherwise, return false.
  return !controller->is_close_requested_ && state == ReadableStreamState_Readable;
}

bool readableStreamDefaultControllerShouldCallPull(ReadableStreamDefaultController *controller) {
  // https://streams.spec.whatwg.org/#readable-stream-default-controller-should-call-pull
  // 1. Let stream be controller.[[controlledReadableStream]].

  // 2. If ! ReadableStreamDefaultControllerCanCloseOrEnqueue(controller) is
  //    false, return false.
  if (!readableStreamDefaultControllerCanCloseOrEnqueue(controller)) {
    return false;
  }

  // 3. If controller.[[started]] is false, return false.
  if (controller->started_ == false) {
    return false;
  }

  // 4. If ! IsReadableStreamLocked(stream) is true and !
  //    ReadableStreamGetNumReadRequests(stream) > 0, return true.
  if (controller->stream_->isLocked() && controller->stream_->getNumReadRequests() > 0) {
    return true;
  }

  // 5. Let desiredSize be ! ReadableStreamDefaultControllerGetDesiredSize
  //    (controller).
  // 6. Assert: desiredSize is not null.
  // 7. If desiredSize > 0, return true.
  // 8. Return false.
  auto desiredSize = controller->getDesiredSize();
  return desiredSize > 0;
}

void readableStreamDefaultControllerCallPullIfNeeded(v8::Isolate *isolate,
                                                     ReadableStreamDefaultController *controller) {
  // https://streams.spec.whatwg.org/#readable-stream-default-controller-call-pull-if-needed
  // 1. Let shouldPull be ! ReadableStreamDefaultControllerShouldCallPull(
  //    controller).
  bool shouldPull = readableStreamDefaultControllerShouldCallPull(controller);
  // 2. If shouldPull is false, return.
  if (!shouldPull) {
    hlogd("readableStreamDefaultControllerCallPullIfNeeded: shouldPull is false");
    return;
  }

  // 3. If controller.[[pulling]] is true,
  if (controller->pulling_) {
    hlogd("readableStreamDefaultControllerCallPullIfNeeded: pull_again_ set true");
    controller->pull_again_ = true;
    return;
  }

  // 4. Assert: controller.[[pullAgain]] is false.
  // 5. Set controller.[[pulling]] to true.
  controller->pulling_ = true;

  // 6. Let pullPromise be the result of performing
  //    controller.[[pullAlgorithm]].
  auto controller_obj = controller->js_object_.Get(isolate);
  auto pullPromise = controller->source_->call_pull(controller_obj);
  v8wrap::promise_then(
      isolate, pullPromise,
      v8wrap::new_function(isolate, controller_pullAlgorithm_resolved, controller),
      v8wrap::new_function(isolate, controller_pullAlgorithm_rejected, controller));
}

void readableStreamDefaultControllerEnqueueValueWithSize(
    const v8::FunctionCallbackInfo<v8::Value> &args, ReadableStreamDefaultController *controller,
    v8::Local<v8::Value> chunk, int64_t chunkSize) {
  auto isolate = args.GetIsolate();
  // https://streams.spec.whatwg.org/#enqueue-value-with-size
  // 3. If ! IsFiniteNonNegativeNumber(size) is false, throw a RangeError
  //    exception.
  if (chunkSize < 1) {
    auto exception =
        v8::Exception::RangeError(v8wrap::new_string(isolate, "size must be a positive number"));
    readableStreamDefaultControllerError(controller, exception);
    return;
  }
  // 4. Append Record {[[value]]: value, [[size]]: size} as the last element of
  //    container.[[queue]].
  controller->enqueue(isolate, chunk, chunkSize);

  // 5. Set container.[[queueTotalSize]] to container.[[queueTotalSize]] + size.
  controller->queue_total_size_ += chunkSize;
}

void readableStreamDefaultControllerError(ReadableStreamDefaultController *controller,
                                          v8::Local<v8::Value> error) {
  // https://streams.spec.whatwg.org/#readable-stream-default-controller-error
  // 1. Let stream be controller.[[controlledReadableStream]].
  auto stream = controller->stream_;

  // 2. If stream.[[state]] is not "readable", return.
  if (stream->state_ != ReadableStreamState_Readable) {
    return;
  }

  // TODO(fxh): 3. Perform ! ResetQueue(controller).
  // TODO(fxh): 4. Perform ! ReadableStreamDefaultControllerClearAlgorithms(controller).
  // TODO(fxh): 5. Perform ! ReadableStreamError(stream, e).

  printf("readableStreamDefaultControllerError\n");
}

static void controller_startAlgorithm_resolved(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto controller = v8wrap::get_ptr<ReadableStreamDefaultController>(args);
  // https://streams.spec.whatwg.org/#set-up-readable-stream-default-controller
  //  11. Upon fulfillment of startPromise,
  //    a. Set controller.[[started]] to true.
  //    b. Assert: controller.[[pulling]] is false.
  //    c. Assert: controller.[[pullAgain]] is false.
  controller->started_ = true;
  hlogd("controller_startAlgorithm_resolved: controller->started_ = true");

  //    d. Perform ! ReadableStreamDefaultControllerCallPullIfNeeded(
  //       controller).
  readableStreamDefaultControllerCallPullIfNeeded(args.GetIsolate(), controller);
}

static void controller_startAlgorithm_rejected(const v8::FunctionCallbackInfo<v8::Value> &args) {
  // https://streams.spec.whatwg.org/#set-up-readable-stream-default-controller
  //  12. Upon rejection of startPromise with reason r,
  //    a. Perform ! ReadableStreamDefaultControllerError(controller, r).
  auto controller = v8wrap::get_ptr<ReadableStreamDefaultController>(args);
  if (args.Length() > 0) {
    auto error = args[0];
    readableStreamDefaultControllerError(controller, error);
  }
  readableStreamDefaultControllerError(controller, v8::Local<v8::Value>());
}

void setupReadableStreamDefaultControllerFromSource(const v8::FunctionCallbackInfo<v8::Value> &args,
                                                    ReadableStream *rs) {
  auto isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();

  // get request scope
  auto reqScope = v8rt::getRequestScope(context);
  if (reqScope == nullptr) {
    v8wrap::throw_type_error(
        isolate, "setupReadableStreamDefaultControllerFromSource: request scope is null");
    return;
  }
  // https://streams.spec.whatwg.org/#set-up-readable-stream-default-controller
  // 1. Assert: stream.[[readableStreamController]] is undefined.
  if (rs->controller_ != nullptr) {
    v8wrap::throw_type_error(isolate, "controller is not null");
    return;
  }

  auto controller = v8rt::allocObject<ReadableStreamDefaultController>(isolate);
  hlogd("setupReadableStreamDefaultControllerFromSource, controller:%p", controller);

  // 2. Set controller.[[controlledReadableStream]] to stream.
  controller->stream_ = rs;

  // TODO(fxh): 3. Set controller.[[queue]] and controller.[[queueTotalSize]] to undefined,
  //    then perform ! ResetQueue(controller).
  // These steps are performed by the constructor, so just check that nothing
  // interfered.

  // TODO(fxh): 5. Set controller.[[strategySizeAlgorithm]] to sizeAlgorithm and
  //    controller.[[strategyHWM]] to highWaterMark.
  // 6. Set controller.[[pullAlgorithm]] to pullAlgorithm.
  // 7. Set controller.[[cancelAlgorithm]] to cancelAlgorithm.
  // 8. Set stream.[[readableStreamController]] to controller.
  controller->source_ = UnderlyingSource::setup(args[0].As<v8::Object>());
  rs->controller_ = controller;

  v8::Local<v8::Value> controller_obj;
  if (!v8wrap::IsolateData::NewInstance(context, CLASS_READABLE_STREAM_DEFAULT_CONTROLLER,
                                        &controller_obj, controller)) {
    v8wrap::throw_type_error(isolate, "create ReadableStreamDefaultController object failed");
    return;
  }
  controller->js_object_.Set(isolate, controller_obj.As<v8::Object>());

  // 9. Let startResult be the result of performing startAlgorithm. (This may
  //    throw an exception.)
  // 10. Let startPromise be a promise resolved with startResult.
  // The conversion of startResult to a promise happens inside start_algorithm
  // in this implementation.
  auto startPromise = controller->source_->call_start(controller_obj.As<v8::Object>());
  hlogd("setupReadableStreamDefaultControllerFromSource:callStartAlgorithm, controller:%p",
        controller);
  v8wrap::promise_then(
      isolate, startPromise,
      v8wrap::new_function(isolate, controller_startAlgorithm_resolved, controller),
      v8wrap::new_function(isolate, controller_startAlgorithm_rejected, controller));
}

}  // namespace v8serviceworker
