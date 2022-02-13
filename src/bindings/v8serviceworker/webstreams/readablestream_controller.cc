/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_controller.h>
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

static void readablestream_controller_js_close(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void readablestream_controller_js_enqueue(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void readablestream_controller_js_error(const v8::FunctionCallbackInfo<v8::Value> &args) {}

namespace v8serviceworker {

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

static void controller_startAlgorithm_resolved(const v8::FunctionCallbackInfo<v8::Value> &args) {
  printf("controller_startAlgorithm_resolved\n");
}

static void controller_startAlgorithm_rejected(const v8::FunctionCallbackInfo<v8::Value> &args) {
  printf("controller_startAlgorithm_rejected\n");
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

  // 1. Assert: stream.[[readableStreamController]] is undefined.
  if (rs->controller_ != nullptr) {
    v8wrap::throw_type_error(isolate,
                             "setupReadableStreamDefaultControllerFromSource: controller is not "
                             "null");
    return;
  }

  // create ReadableStreamDefaultController native object
  auto controller = v8rt::allocObject<ReadableStreamDefaultController>(isolate);

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

  printf("setupReadableStreamDefaultControllerFromSource\n");

  // create ReadableStreamDefaultController js object
  v8::Local<v8::Value> controller_obj;
  if (!v8wrap::IsolateData::NewInstance(context, CLASS_READABLE_STREAM_DEFAULT_CONTROLLER,
                                        &controller_obj)) {
    v8wrap::throw_type_error(isolate,
                             "setupReadableStreamDefaultControllerFromSource: create "
                             "ReadableStreamDefaultController object failed");
    return;
  }
  v8wrap::set_ptr(controller_obj.As<v8::Object>(), controller);

  // 9. Let startResult be the result of performing startAlgorithm. (This may
  //    throw an exception.)
  // 10. Let startPromise be a promise resolved with startResult.
  // The conversion of startResult to a promise happens inside start_algorithm
  // in this implementation.
  auto startPromise = controller->source_->call_start(controller_obj.As<v8::Object>());

  v8wrap::promise_then(
      isolate, startPromise,
      v8wrap::new_function(isolate, controller_startAlgorithm_resolved, controller),
      v8wrap::new_function(isolate, controller_startAlgorithm_rejected, controller));
}

}  // namespace v8serviceworker
