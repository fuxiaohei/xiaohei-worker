/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/event/fetch_event.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <common/common.h>
#include <hv/hlog.h>
#include <runtime/v8rt/v8js_context.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_value.h>
#include <webapi/event_target.h>
#include <webapi/fetch/fetch_response.h>

#include <string>

namespace v8serviceworker {

static void fetch_event_js_respondWith(const v8::FunctionCallbackInfo<v8::Value> &args) {
  // make event canceled after respondWith once
  auto event = v8wrap::get_ptr<webapi::Event>(args.Holder());
  event->cancel();

  if (v8wrap::valid_arglen(args, 1, "respondWith: ")) {
    return;
  }

  auto isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  auto reqScope = v8rt::getRequestScope(context);
  if (reqScope == nullptr) {
    v8wrap::throw_type_error(isolate, "respondWith: request scope is null");
    return;
  }

  if (args[0]->IsPromise()) {
    isolate->PerformMicrotaskCheckpoint();
    hlogi("fetch_event_js_respondWith check promise, reqScope:%p", reqScope);

    auto promise = args[0].As<v8::Promise>();
    if (promise->State() == v8::Promise::kFulfilled) {
      auto value = promise->Result();
      if (!v8wrap::IsolateData::IsInstanceOf(context, value, CLASS_FETCH_RESPONSE)) {
        v8wrap::throw_type_error(isolate, "respondWith: argument must be a Response");
        return;
      }
      auto response = v8wrap::get_ptr<webapi::FetchResponse>(value.As<v8::Object>());
      reqScope->set_response(response);
      return;
    }
    if (promise->State() == v8::Promise::kRejected) {
      auto value = promise->Result();
      std::string errormsg = v8wrap::to_string(context, value);
      reqScope->set_error_msg(common::ERROR_V8JS_THREW_EXCEPTION, errormsg);
      return;
    }

    isolate->PerformMicrotaskCheckpoint();
    hlogi("fetch_event_js_respondWith save promise, reqScope:%p", reqScope);

    auto jsContext = v8rt::getJsContext(context);
    if (jsContext == nullptr) {
      reqScope->set_error_msg(common::ERROR_V8JS_THREW_EXCEPTION, "invalid request context");
      hlogi("fetch_event_js_respondWith invalid request context, reqScope:%p", reqScope);
      return;
    }
    jsContext->save_response_promise(promise);
    return;
  }

  if (!v8wrap::IsolateData::IsInstanceOf(context, args[0], CLASS_FETCH_RESPONSE)) {
    v8wrap::throw_type_error(isolate, "respondWith: argument must be a Response");
    return;
  }
  auto response = v8wrap::get_ptr<webapi::FetchResponse>(args[0].As<v8::Object>());
  reqScope->set_response(response);
  hlogi("fetch_event_js_respondWith save response, response:%p, reqScope:%p", response, reqScope);
}

static void fetch_event_js_waitUntil(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void fetch_event_js_request_getter(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto *isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();

  // get request scope
  auto reqScope = v8rt::getRequestScope(context);
  if (reqScope == nullptr) {
    v8wrap::throw_type_error(isolate, "event.request: request scope is null");
    return;
  }

  v8::Local<v8::Value> requestObject;
  if (!v8wrap::IsolateData::NewInstance(context, CLASS_FETCH_REQUEST, &requestObject)) {
    v8wrap::throw_type_error(isolate, "event.request: failed to create request object");
    return;
  }

  auto fetchRequest = reqScope->create_fetch_request();
  v8wrap::set_ptr(requestObject.As<v8::Object>(), fetchRequest);

  args.GetReturnValue().Set(requestObject);
}

void register_fetch_event(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuider) {
  auto isolate = isolateData->get_isolate();
  v8wrap::ClassBuilder fetchEventBuilder(isolate, CLASS_FETCH_EVENT);
  fetchEventBuilder.setConstructor(nullptr);
  fetchEventBuilder.setMethod("respondWith", fetch_event_js_respondWith);
  fetchEventBuilder.setMethod("waitUntil", fetch_event_js_waitUntil);
  fetchEventBuilder.setAccessorProperty("request", fetch_event_js_request_getter, nullptr, nullptr);
  auto fetch_event_template = fetchEventBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_FETCH_EVENT, fetch_event_template);
  classBuider->setMethod(CLASS_FETCH_EVENT, fetch_event_template);
}

}  // namespace v8serviceworker
