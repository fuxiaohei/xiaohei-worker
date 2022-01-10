/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <runtime/runtime.h>
#include <v8.h>

#define V8_JS_ISOLATE_MEMORY_LIMIT 128  // in mb

namespace xhworker {
namespace runtime {

class V8Runtime : public Runtime {
 public:
  RuntimeContext *get_context();
  void recycle_context(RuntimeContext *context);
  int compile(const std::string &content, const std::string &origin);
  std::string get_compile_error_message() { return compile_error_message_; }

 private:
  friend class Runtime;
  explicit V8Runtime(const Options &options);
  ~V8Runtime();

 private:
  std::string content_;
  std::string origin_;
  std::string compile_error_message_;

  v8::Isolate *isolate_ = nullptr;
  v8::ArrayBuffer::Allocator *allocator_ = nullptr;
};

}  // namespace runtime
}  // namespace xhworker