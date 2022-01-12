/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/event/event_listener.h>
#include <hv/hlog.h>

namespace v8serviceworker {

void JsEventListener::call(webapi::Event *event) {
  hlogw("JsEventListener::call");
  /*
auto *req_scope = static_cast<workify::RequestScope *>(event->data);

v8::HandleScope handle_scope(isolate);
v8::TryCatch try_catch(isolate);
auto context = isolate->GetCurrentContext();

// create event object
v8::Local<v8::Value> event_obj;
if (!runner::V8Js::newClassInstance(context, "FetchEvent", &event_obj)) {
  req_scope->set_error_msg(common::ERROR_V8JS_THREW_EXCEPTION, "FetchEvent is undefined");
  return;
}
v8wrap::set_ptr(event_obj.As<v8::Object>(), event);

// call function
auto fn = function.Get(isolate);
v8::Local<v8::Value> args[1] = {event_obj};
auto maybe_value = fn->Call(context, context->Global(), 1, args);

// handle exception
if (try_catch.HasCaught()) {
  std::string error, stack;
  v8wrap::get_exception(context, &try_catch, &error, &stack);
  req_scope->set_error_msg(common::ERROR_V8JS_THREW_EXCEPTION, error, stack);

  hlogd("v8js: call js function error:%s, stack:%s", error.c_str(), stack.c_str());
  return;
}

// handle run result
if (maybe_value.IsEmpty()) {
  return;
}

auto value = maybe_value.ToLocalChecked();
if (value->IsPromise()) {
  // if throw Exception is async function
  // js:
  //  async function() { throw new Error("xxxx") }
  // it calls promise rejected

  auto promise = value.As<v8::Promise>();

  // if promise is rejected, it will set req_scope->error_msg
  if (promise->State() == v8::Promise::kRejected) {
    std::string errmsg = v8wrap::to_string(context, promise->Result());
    req_scope->set_error_msg(common::ERROR_V8JS_THREW_EXCEPTION, errmsg);
    return;
  }

  // force to run microtask once
  if (promise->State() == v8::Promise::kPending) {
    isolate->PerformMicrotaskCheckpoint();
  }
}*/
}
}  // namespace v8serviceworker