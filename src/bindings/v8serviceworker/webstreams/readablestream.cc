/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/bytestream_controller.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_controller.h>
#include <bindings/v8serviceworker/webstreams/readablestream_reader.h>
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

// --- ReadableStream Js Methods --

static void readablestream_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {
  // https://streams.spec.whatwg.org/#rs-constructor
  //  1. Perform ! InitializeReadableStream(this).
  auto isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  auto stream = v8rt::allocObject<ReadableStream>(isolate);

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

v8::Local<v8::FunctionTemplate> create_readablestream_template(v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder rsBuilder(isolateData->get_isolate(), CLASS_READABLE_STREAM);
  rsBuilder.setConstructor(readablestream_js_constructor);
  auto rsTemplate = rsBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_READABLE_STREAM, rsTemplate);

  return rsTemplate;
}

};  // namespace v8serviceworker
