/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/fetch/fetch.h>
#include <bindings/v8serviceworker/fetch/fetch_request.h>
#include <bindings/v8serviceworker/fetch/fetch_response.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <hv/hlog.h>
#include <runtime/v8rt/v8js_fetch.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>

#include <string>
#include <utility>
#include <vector>

namespace v8serviceworker {

static void js_global_fetch(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (v8wrap::valid_arglen(args, 1, "fetch: ")) {
    return;
  }

  auto *isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  auto resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
  args.GetReturnValue().Set(resolver->GetPromise());

  auto reqScope = v8rt::getRequestScope(context);
  if (reqScope == nullptr) {
    v8wrap::throw_type_error(isolate, "fetch: request scope is null");
    return;
  }

  auto fetchContext = v8rt::V8FetchContext::Create(reqScope, resolver);
  fetchContext->set_url("http://127.0.0.1:18888/hello");
}

void register_fetch_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  auto responseTemplate = create_response_template(isolateData);
  classBuilder->setMethod(CLASS_FETCH_RESPONSE, responseTemplate);

  auto requestTmeplate = create_request_template(isolateData);
  classBuilder->setMethod(CLASS_FETCH_REQUEST, requestTmeplate);

  classBuilder->setMethod("fetch", js_global_fetch);
}

webapi::FetchHeaders *init_headers(v8::Isolate *isolate, v8::Local<v8::Value> value) {
  auto *headers = v8rt::allocObject<webapi::FetchHeaders>(isolate);
  std::vector<std::pair<std::string, std::string>> header_values;

  if (v8wrap::convert_object_map(isolate->GetCurrentContext(), value, &header_values)) {
    for (auto &item : header_values) {
      headers->append(item.first, item.second);
    }
    return headers;
  }

  v8wrap::throw_type_error(isolate, "invalid sequence");
  return headers;
}

}  // namespace v8serviceworker
