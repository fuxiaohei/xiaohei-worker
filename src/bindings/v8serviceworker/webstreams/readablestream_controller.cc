/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream_controller.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_value.h>

static void readablestream_controller_js_constructor(
    const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void readablestream_controller_js_desiredSize_getter(
    const v8::FunctionCallbackInfo<v8::Value> &args) {}

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
}  // namespace v8serviceworker