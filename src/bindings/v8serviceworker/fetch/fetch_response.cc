/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/fetch/fetch.h>
#include <bindings/v8serviceworker/fetch/fetch_response.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>
#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>
#include <webapi/fetch/fetch_response.h>

#include <string>

namespace v8serviceworker {

static void fetch_response_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto isolate = args.GetIsolate();
  auto response = v8rt::allocObject<webapi::FetchResponse>(isolate);
  v8wrap::set_ptr(args.Holder(), response);

  if (args.Length() >= 1) {
    // TODO(fuxiaohei): finish all response api

    if (args[0]->IsString()) {
      std::string body_text = v8wrap::get_string(isolate, args[0]);
      response->setBody(body_text);
    }
  }
  if (args.Length() >= 2) {
    if (!args[1]->IsObject()) {
      v8wrap::throw_type_error(isolate, "Response: second argument must be an object");
      return;
    }
    auto object = args[1].As<v8::Object>();

    // status
    if (v8wrap::has_property(object, "status")) {
      int status = v8wrap::get_int32(v8wrap::get_property(object, "status"));
      response->setStatus(status);
    }

    // TODO(fuxiaohei): statusText

    // headers
    if (v8wrap::has_property(object, "headers")) {
      auto headers = init_headers(isolate, v8wrap::get_property(object, "headers"));
      response->setHeaders(headers);
    }
  }
}
static void fetch_response_js_redirect(const v8::FunctionCallbackInfo<v8::Value> &args) {}

v8::Local<v8::FunctionTemplate> create_response_template(v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder responseBuilder(isolateData->get_isolate(), CLASS_FETCH_RESPONSE);
  responseBuilder.setConstructor(fetch_response_js_constructor);
  responseBuilder.setStaticMethod("redirect", fetch_response_js_redirect);
  auto responseTemplate = responseBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_FETCH_RESPONSE, responseTemplate);

  return responseTemplate;
}

}  // namespace v8serviceworker
