/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <core/request_scope.h>
#include <runtime/runtime.h>
#include <v8.h>

#include <queue>
#include <string>

#define V8_JS_ISOLATE_MEMORY_LIMIT 128  // in mb

namespace v8rt {

using runtime::Options;
using runtime::Runtime;
using runtime::RuntimeContext;

class V8JsContext;

class V8Runtime : public Runtime {
 public:
  RuntimeContext *get_context();
  void recycle_context(RuntimeContext *context);
  int compile(const std::string &content, const std::string &origin);
  std::string get_compile_error_message() { return compile_error_message_; }
  v8::Isolate *get_isolate() { return isolate_; }

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

  std::queue<RuntimeContext *> context_queue_;
};

common::Heap *getHeap(v8::Local<v8::Context> context);

core::RequestScope *getRequestScope(v8::Local<v8::Context> context);

V8JsContext *getJsContext(v8::Local<v8::Context> context);

template <class T>
static T *allocObject(v8::Local<v8::Context> context) {
  auto heapObject = getHeap(context);
  if (heapObject == nullptr) {
    return nullptr;
  }
  return heapObject->alloc<T>();
}

template <class T>
static T *allocObject(v8::Isolate *isolate) {
  return allocObject<T>(isolate->GetCurrentContext());
}

}  // namespace v8rt
