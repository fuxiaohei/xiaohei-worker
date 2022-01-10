/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <runtime/runtime.h>
#include <v8.h>

#include <string>

namespace xhworker {
namespace runtime {

class V8Runtime;

class V8JsContext : public RuntimeContext {
 public:
  static V8JsContext *Create(V8Runtime *rt, const std::string &content, const std::string &origin);

 public:
  int get_error_code() const { return error_code_; }

  void set_compiled_result(const std::string &result);
  void set_exception(const std::string &msg, const std::string &stack);

 private:
  friend class V8Runtime;
  const std::string &get_exception_msg() const { return exception_msg_; }
  const std::string &get_exception_stack() const { return exception_stack_; }

  V8JsContext(V8Runtime *rt, v8::Local<v8::Context> context);
  ~V8JsContext();

 private:
  int error_code_ = 0;
  std::string compiled_result_;
  std::string exception_msg_;
  std::string exception_stack_;
};

}  // namespace runtime
}  // namespace xhworker