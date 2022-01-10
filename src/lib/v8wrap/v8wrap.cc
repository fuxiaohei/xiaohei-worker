/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#include <libplatform/libplatform.h>
#include <v8wrap/js_value.h>
#include <v8wrap/v8wrap.h>

#include <memory>
#include <utility>

static std::unique_ptr<v8::Platform> pt = v8::platform::NewDefaultPlatform();

static bool is_initialized = false;

namespace v8wrap {

void init(const std::string &icu_file) {
  if (is_initialized) {
    return;
  }
  is_initialized = true;

  if (!icu_file.empty()) {
    v8::V8::InitializeICU(icu_file.c_str());
  }

  v8::V8::InitializePlatform(pt.get());
  v8::V8::Initialize();
}

void shutdown() {
  if (!is_initialized) {
    return;
  }
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
}

// ----

v8::Local<v8::Context> new_context(v8::Isolate *isolate) { return v8::Context::New(isolate); }

v8::Local<v8::Context> new_context(v8::Isolate *isolate,
                                   v8::Local<v8::ObjectTemplate> global_template) {
  return v8::Context::New(isolate, nullptr, global_template);
}

// ----

Result run_script(v8::Isolate *isolate, const std::string &script) {
  v8::Local<v8::Context> context = new_context(isolate);
  return run_script(context, script);
}

Result run_script(v8::Local<v8::Context> context, const std::string &script) {
  auto *isolate = context->GetIsolate();
  v8::EscapableHandleScope handle_scope(isolate);  // use this scope to escape the context
  v8::TryCatch try_catch(isolate);
  v8::Context::Scope context_scope(context);

  v8::Local<v8::String> script_value = new_string(isolate, script);
  v8::Local<v8::Script> compiled_script;

  // resultå
  Result res;

  // compile script
  if (!v8::Script::Compile(context, script_value).ToLocal(&compiled_script)) {
    res.is_exception = true;
    get_exception(context, &try_catch, &res.exception_message, &res.exception_stack);
    return std::move(res);
  }

  // run script
  v8::Local<v8::Value> result;
  if (!compiled_script->Run(context).ToLocal(&result)) {
    res.is_exception = true;
    get_exception(context, &try_catch, &res.exception_message, &res.exception_stack);
    return std::move(res);
  }

  // return value
  res.value = handle_scope.Escape(result);
  return std::move(res);
}

}  // namespace v8wrap
