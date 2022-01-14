/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/fetch/fetch.h>
#include <bindings/v8serviceworker/fetch/fetch_request.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>
#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>
#include <webapi/fetch/fetch_request.h>

namespace v8serviceworker {

static void fetch_request_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void fetch_request_js_method_getter(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto *isolate = args.GetIsolate();
  auto *request = v8wrap::get_ptr<webapi::FetchRequest>(args.Holder());
  args.GetReturnValue().Set(v8wrap::new_string(isolate, request->get_method()));
}

v8::Local<v8::FunctionTemplate> create_request_template(v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder requestBuilder(isolateData->get_isolate(), CLASS_FETCH_REQUEST);
  requestBuilder.setConstructor(fetch_request_js_constructor);
  requestBuilder.setAccessorProperty("method", fetch_request_js_method_getter, nullptr);
  auto requestTemplate = requestBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_FETCH_REQUEST, requestTemplate);

  return requestTemplate;
}

}  // namespace v8serviceworker
