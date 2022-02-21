/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/bytestream_controller.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_byob_reader.h>
#include <bindings/v8serviceworker/webstreams/readablestream_default_controller.h>
#include <bindings/v8serviceworker/webstreams/readablestream_default_reader.h>
#include <bindings/v8serviceworker/webstreams/strategy.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

// --- ReadableStream ---

size_t ReadableStream::getDesiredSize() {
  hlogd("ReadableStream::getDesiredSize(), highWaterMark: %d, queue_total_size:%d", highWaterMark_,
        get_queue_total_size());
  return highWaterMark_ - get_queue_total_size();
}

size_t ReadableStream::getNumReadRequests() {
  printf("ReadableStream::getNumReadRequests():%p\n", reader_);
  assert(reader_ != nullptr);
  return reader_->getReadRequestsSize();
}

int64_t ReadableStream::callSizeAlgorithm(v8::Local<v8::Context> context) {
  auto fn = sizeAlgorithm_.Get(context->GetIsolate());

  v8::Local<v8::Value> args[0];
  auto maybeResult = fn->Call(context, context->Global(), 0, args);
  if (maybeResult.IsEmpty()) {
    return -1;
  }

  auto result = maybeResult.ToLocalChecked();
  if (result->IsNumber()) {
    return result->IntegerValue(context).FromJust();
  }
  return -1;
}

int64_t ReadableStream::get_queue_total_size() {
  if (controller_ != nullptr) {
    return controller_->queue_total_size_;
  }
  return 0;
}

void ReadableStream::setClose(v8::Isolate *isolate) {
  // https://streams.spec.whatwg.org/#readable-stream-close
  // 1. Assert: stream.[[state]] is "readable".

  // 2. Set stream.[[state]] to "closed".
  state_ = ReadableStreamState_Closed;

  // 3. Let reader be stream.[[reader]].
  // 4. If reader is undefined, return.
  if (reader_ == nullptr) {
    return;
  }

  // TODO(fxh): 5. If ! IsReadableStreamDefaultReader(reader) is true,
  // 6. Resolve reader.[[closedPromise]] with undefined.
}

void ReadableStream::setError(v8::Isolate *isolate, v8::Local<v8::Value> error) {
  // https://streams.spec.whatwg.org/#readable-stream-error
  // 1. Assert: stream.[[state]] is "readable".
  if (state_ != ReadableStreamState_Readable) {
    return;
  }
  // 2. Set stream.[[state]] to "errored".
  state_ = ReadableStreamState_Errored;
  // 3. Set stream.[[storedError]] to e.
  stored_error_.Set(isolate, error);
  // 4. Let reader be stream.[[reader]].
  // 5. If reader is undefined, return.
  if (!reader_) {
    return;
  }
  // TODO(fxh): 6,7,8,9
}

v8::Local<v8::Value> ReadableStream::getError(v8::Isolate *isolate) {
  return stored_error_.Get(isolate);
}

v8::Local<v8::Object> ReadableStream::CreateReadResult(v8::Isolate *isolate,
                                                       v8::Local<v8::Value> value, bool done) {
  auto object = v8::Object::New(isolate);
  v8wrap::set_property(object, "value", value);
  v8wrap::set_property(object, "done", v8::Boolean::New(isolate, done));
  return object;
}

// --- ReadableStream Js Methods --

static void readablestream_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {
  // https://streams.spec.whatwg.org/#rs-constructor
  //  1. Perform ! InitializeReadableStream(this).
  auto isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  auto stream = v8rt::allocObject<ReadableStream>(isolate);
  v8wrap::set_ptr(args.Holder(), stream);

  v8::Local<v8::Object> underlyingSource;
  if (args.Length() >= 1) {
    if (!args[0]->IsObject()) {
      v8wrap::throw_type_error(args.GetIsolate(),
                               "first argument must be an UnderlyingSource object");
      return;
    }
    underlyingSource = args[0].As<v8::Object>();
  } else {
    underlyingSource = v8::Object::New(isolate);  // use empty object instead of undefined
  }

  // 2. Let size be ? GetV(strategy, "size").
  // 3. Let highWaterMark be ? GetV(strategy, "highWaterMark").
  v8::Local<v8::Object> strategy;
  if (args.Length() >= 2) {
    if (!args[1]->IsObject()) {
      v8wrap::throw_type_error(args.GetIsolate(), "second argument must be a Strategy object");
      return;
    }
    strategy = create_countqueuing_strategy_instance(context, args[1]);
  }

  // 4. Let type be ? GetV(underlyingSource, "type").
  auto underlyingSourceType = v8wrap::get_property(underlyingSource, "type");

  if (underlyingSourceType->IsUndefined()) {
    // 7. Otherwise, if type is undefined,
    //   a. Let sizeAlgorithm be ? MakeSizeAlgorithmFromSizeFunction(size).
    auto sizeAlgorithm = make_countqueuing_strategy_size_algorithm(strategy);
    stream->setSizeAlgorithm(sizeAlgorithm);
    //   b. If highWaterMark is undefined, let highWaterMark be 1.
    //   c. Set highWaterMark to ? ValidateAndNormalizeHighWaterMark(
    //      highWaterMark).
    stream->setHighWaterMark(make_countqueuing_strategy_high_water_mark(strategy, 1));
    // 4. Perform ? SetUpReadableStreamDefaultControllerFromUnderlyingSource
    //  (this, underlyingSource, highWaterMark, sizeAlgorithm).
    setupReadableStreamDefaultControllerFromSource(args, stream);
    return;
  }

  // 5. Let typeString be ? ToString(type).
  // 6. If typeString is "bytes",
  auto typeStr = v8wrap::get_string(isolate, underlyingSourceType);
  if (typeStr != "bytes") {
    v8wrap::throw_range_error(args.GetIsolate(), "type must be 'bytes'");
    return;
  }
  stream->setHighWaterMark(make_bytelengthqueuing_strategy_high_water_mark(strategy, 1));
  setupReadableByteStreamControllerFromSource(args, stream);
}

static void readablestream_js_locked_getter(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto stream = v8wrap::get_ptr<ReadableStream>(args.Holder());
  args.GetReturnValue().Set(stream->isLocked());
}

static void readablestream_js_getReader(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (args.Length() > 0) {
    if (!args[0]->IsObject()) {
      v8wrap::throw_type_error(args.GetIsolate(), "first argument must be an options object");
      return;
    }
    auto options = args[0].As<v8::Object>();
    if (v8wrap::has_property(options, "mode")) {
      auto mode = v8wrap::get_property(options, "mode");
      if (!mode->IsString()) {
        v8wrap::throw_type_error(args.GetIsolate(), "mode must be a string");
        return;
      }
      auto modeStr = v8wrap::get_string(args.GetIsolate(), mode);
      if (modeStr == "byob") {
        auto stream = v8wrap::get_ptr<ReadableStream>(args.Holder());
        auto obj =
            ReadableStreamBYOBReader::Aqcuire(args.GetIsolate()->GetCurrentContext(), stream);
        args.GetReturnValue().Set(obj);
        return;
      }
    }
  }

  auto stream = v8wrap::get_ptr<ReadableStream>(args.Holder());
  auto obj = ReadableStreamDefaultReader::Aqcuire(args.GetIsolate()->GetCurrentContext(), stream);
  args.GetReturnValue().Set(obj);
}

static void readablestream_js_pipeThrough(const v8::FunctionCallbackInfo<v8::Value> &args) {}
static void readablestream_js_pipeTo(const v8::FunctionCallbackInfo<v8::Value> &args) {}
static void readablestream_js_tee(const v8::FunctionCallbackInfo<v8::Value> &args) {}
static void readablestream_js_cancel(const v8::FunctionCallbackInfo<v8::Value> &args) {}

v8::Local<v8::FunctionTemplate> create_readablestream_template(v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder rsBuilder(isolateData->get_isolate(), CLASS_READABLE_STREAM);
  rsBuilder.setConstructor(readablestream_js_constructor);
  rsBuilder.setAccessorProperty("locked", readablestream_js_locked_getter, nullptr);
  rsBuilder.setMethod("getReader", readablestream_js_getReader);
  rsBuilder.setMethod("pipeThrough", readablestream_js_pipeThrough);
  rsBuilder.setMethod("pipeTo", readablestream_js_pipeTo);
  rsBuilder.setMethod("tee", readablestream_js_tee);
  rsBuilder.setMethod("cancel", readablestream_js_cancel);
  auto rsTemplate = rsBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_READABLE_STREAM, rsTemplate);

  return rsTemplate;
}

};  // namespace v8serviceworker
