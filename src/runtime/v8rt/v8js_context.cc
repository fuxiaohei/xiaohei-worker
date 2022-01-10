/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <lib/common/common.h>
#include <runtime/v8rt/v8js_context.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_value.h>
#include <v8wrap/v8wrap.h>

namespace xhworker {
namespace runtime {

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

  std::string result_string = v8wrap::get_string(isolate, result);
  jsContext->set_compiled_result(result_string);
  return jsContext;
}

// -- methods

V8JsContext::V8JsContext(V8Runtime *rt, v8::Local<v8::Context> context) {}
V8JsContext::~V8JsContext() {}

void V8JsContext::set_exception(const std::string &msg, const std::string &stack) {
  error_code_ = common::ERROR_RUNTIME_COMPILE_ERROR;
  exception_msg_ = msg;
  exception_stack_ = stack;
}

void V8JsContext::set_compiled_result(const std::string &result) { compiled_result_ = result; }

}  // namespace runtime
}  // namespace xhworker