/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_controller.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_value.h>

static void readablestream_controller_js_constructor(
    const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void readablestream_controller_js_desiredSize_getter(
    const v8::FunctionCallbackInfo<v8::Value> &args) {
  using namespace v8serviceworker;
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

static void setupReadableStreamDefaultController(ReadableStreamDefaultController *controller,
                                                 v8::Local<v8::Object> underlyingSource) {
  printf("setupReadableStreamDefaultController\n");
};

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

  // create ReadableStreamDefaultController js object
  v8::Local<v8::Value> controller_obj;
  if (!v8wrap::IsolateData::NewInstance(context, CLASS_READABLE_STREAM_DEFAULT_CONTROLLER,
                                        &controller_obj)) {
    v8wrap::throw_type_error(isolate,
                             "setupReadableStreamDefaultControllerFromSource: create "
                             "ReadableStreamDefaultController object failed");
    return;
  }

  // create ReadableStreamDefaultController native object
  auto controller = v8rt::allocObject<ReadableStreamDefaultController>(isolate);
  rs->controller_ = controller;
  controller->stream_ = rs;
  v8wrap::set_ptr(controller_obj.As<v8::Object>(), controller);

  printf("setupReadableStreamDefaultControllerFromSource\n");

  setupReadableStreamDefaultController(controller, args[0].As<v8::Object>());
}

}  // namespace v8serviceworker