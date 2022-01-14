/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <common/common.h>
#include <hv/hlog.h>
#include <runtime/v8rt/v8js_context.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_value.h>
#include <v8wrap/v8wrap.h>
#include <webapi/global_scope.h>

namespace v8rt {

V8JsContext *V8JsContext::Create(V8Runtime *rt, const std::string &content,
                                 const std::string &origin) {
  auto isolate = rt->get_isolate();
  auto isolateData = v8wrap::IsolateData::Get(isolate);

  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  auto globalTemplate = isolateData->getClassTemplate(CLASS_SERVICEWORKER_GLOBAL_SCOPE);
  if (globalTemplate.IsEmpty()) {
    globalTemplate = v8serviceworker::create_service_worker_global_scope(isolate);
  }

  v8::Local<v8::Context> context = v8wrap::new_context(isolate, globalTemplate->InstanceTemplate());

  v8::TryCatch try_catch(isolate);
  v8::Context::Scope context_scope(context);

  // set origin
  std::string jsname = origin;
  if (jsname.empty()) {
    jsname = "index.js";
  }
  v8::ScriptOrigin script_origin(v8wrap::new_string(isolate, jsname));

  // create js context
  auto *jsContext = new V8JsContext(rt, context);

  // compile
  v8::Local<v8::String> script_value = v8wrap::new_string(isolate, content);
  v8::Local<v8::Script> compiled_script;
  if (!v8::Script::Compile(context, script_value, &script_origin).ToLocal(&compiled_script)) {
    std::string msg, stack;
    v8wrap::get_exception(context, &try_catch, &msg, &stack);
    jsContext->set_exception(msg, stack);
    return jsContext;
  }

  // run script
  v8::Local<v8::Value> result;
  if (!compiled_script->Run(context).ToLocal(&result)) {
    std::string msg, stack;
    v8wrap::get_exception(context, &try_catch, &msg, &stack);
    jsContext->set_exception(msg, stack);
    return jsContext;
  }

  // check listener registered
  jsContext->check_handler_registered();
  if (jsContext->get_error_code() != 0) {
    return jsContext;
  }

  std::string result_string = v8wrap::get_string(isolate, result);
  jsContext->set_compiled_result(result_string);
  return jsContext;
}

// -- methods

V8JsContext::V8JsContext(V8Runtime *rt, v8::Local<v8::Context> context)
    : runtime_(rt), isolate_(context->GetIsolate()) {
  global_scope_ = webapi::ServiceWorkerGlobalScope::create();
  context_.Set(context->GetIsolate(), context);

  v8wrap::set_ptr(context, V8_JS_CONTEXT_JSGLOBAL_INDEX, global_scope_);
  v8wrap::set_ptr(context, V8_JS_CONTEXT_REQUEST_SCOPE_INDEX, nullptr);
  v8wrap::set_ptr(context, V8_JS_CONTEXT_SELF_INDEX, this);

  hlogi("v8js: create js context: %p", this);
}

V8JsContext::~V8JsContext() { hlogi("v8js: dispose jsContext :%p", this); }

void V8JsContext::set_exception(const std::string &msg, const std::string &stack) {
  error_code_ = common::ERROR_RUNTIME_COMPILE_ERROR;
  exception_msg_ = msg;
  exception_stack_ = stack;
}

void V8JsContext::set_compiled_result(const std::string &result) { compiled_result_ = result; }

void V8JsContext::check_handler_registered() {
  if (!global_scope_->event_target_->hasListener("fetch")) {
    error_code_ = common::ERROR_RUNTIME_MISSING_FETCH_HANDLER;
  }
}

int V8JsContext::handle_http_request(core::RequestScope *reqScope) {
  v8::Isolate::Scope isolate_scope(isolate_);
  v8::HandleScope handle_scope(isolate_);

  // enter v8::Context
  auto context = context_.Get(isolate_);
  v8wrap::set_ptr(context, V8_JS_CONTEXT_REQUEST_SCOPE_INDEX, reqScope);
  v8::Context::Scope context_scope(context);

  // create fetch event
  auto fetchEvent = reqScope->create_fetch_event();

  // dispatch event
  hlogd("v8js: handle_request, jsCtx:%p, req_scope:%p, fetchEvent:%p", this, reqScope, fetchEvent);

  // dispatch fetch event
  global_scope_->event_target_->dispatchEvent(fetchEvent);

  // handle response,
  // if response is not promise, it sends response to client
  // if response is promise, it waits for response and return 0
  return reqScope->handle_response();
}

void V8JsContext::recycle() {
  hlogi("v8js: recycle js context: %p", this);
  runtime_->recycle_context(this);
}

void V8JsContext::save_response_promise(v8::Local<v8::Promise> promise) {
  respose_promise_.Set(isolate_, promise);
}

webapi::FetchResponse *V8JsContext::get_promised_respose() {
  if (respose_promise_.IsEmpty()) {
    hlogd("v8js: get_promised_respose, jsCtx:%p, promised_respose is empty", this);
    return nullptr;
  }
  auto promise = respose_promise_.Get(isolate_);
  if (promise->State() == v8::Promise::kFulfilled) {
    return v8wrap::get_ptr<webapi::FetchResponse>(promise->Result().As<v8::Object>());
  }
  // FIXME: rejected?
  return nullptr;
}

}  // namespace v8rt
