/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <runtime/runtime.h>
#include <v8.h>

#include <string>

#define V8_JS_CONTEXT_JSGLOBAL_INDEX 3
#define V8_JS_CONTEXT_REQUEST_SCOPE_INDEX 4
#define V8_JS_CONTEXT_SELF_INDEX 5

namespace webapi {
class ServiceWorkerGlobalScope;
}

namespace xhworker {
namespace v8rt {

class V8Runtime;

using xhworker::runtime::RuntimeContext;

class V8JsContext : public RuntimeContext {
 public:
  static V8JsContext *Create(V8Runtime *rt, const std::string &content, const std::string &origin);

 public:
  int handle_http_request(xhworker::core::RequestScope *reqscope) override;
  int get_error_code() const override { return error_code_; }
  void recycle() override;

 public:
  std::string get_compiled_result() const { return compiled_result_; }

  void set_compiled_result(const std::string &result);
  void set_exception(const std::string &msg, const std::string &stack);

 private:
  friend class V8Runtime;
  const std::string &get_exception_msg() const { return exception_msg_; }
  const std::string &get_exception_stack() const { return exception_stack_; }

  void check_handler_registered();

  V8JsContext(V8Runtime *rt, v8::Local<v8::Context> context);
  ~V8JsContext();

 private:
  int error_code_ = 0;
  std::string compiled_result_;
  std::string exception_msg_;
  std::string exception_stack_;

  v8::Isolate *isolate_ = nullptr;
  v8::Eternal<v8::Context> context_;

  V8Runtime *runtime_ = nullptr;

  webapi::ServiceWorkerGlobalScope *global_scope_ = nullptr;
};

}  // namespace v8rt
}  // namespace xhworker