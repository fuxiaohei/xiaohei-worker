/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>

#include <string>

namespace v8wrap {

void inline set_ptr(v8::Isolate *isolate, uint32_t index, void *ptr) {
  isolate->SetData(index, ptr);
}

void inline set_ptr(v8::Local<v8::Context> context, int index, void *ptr) {
  context->SetAlignedPointerInEmbedderData(index, ptr);
}

void inline set_ptr(v8::Local<v8::Object> handle, void *pointer) {
  assert(!handle.IsEmpty());
  assert(handle->InternalFieldCount() > 0);
  handle->SetAlignedPointerInInternalField(0, pointer);
}

template <class T>
T *get_ptr(v8::Isolate *isolate, uint32_t index) {
  auto value = isolate->GetData(index);
  if (value == nullptr) {
    return nullptr;
  }
  return static_cast<T *>(value);
}

template <class T>
T *get_ptr(v8::Local<v8::Context> context, int index) {
  auto value = context->GetAlignedPointerFromEmbedderData(index);
  if (value == nullptr) {
    return nullptr;
  }
  return static_cast<T *>(value);
}

template <class T>
static T *get_ptr(v8::Local<v8::External> extValue) {
  auto v = extValue->Value();
  return static_cast<T *>(v);
}

template <class T>
static T *get_ptr(v8::Local<v8::Object> handle) {
  assert(!handle.IsEmpty());
  assert(handle->InternalFieldCount() > 0);
  void *ptr = handle->GetAlignedPointerFromInternalField(0);
  return static_cast<T *>(ptr);
}

v8::Local<v8::String> new_string(v8::Isolate *isolate, const char *chars, size_t length);
v8::Local<v8::String> new_string(v8::Isolate *isolate, const std::string &str);
std::string get_string(v8::Isolate *isolate, v8::Local<v8::Value> value);
std::string to_string(v8::Local<v8::Context> context, v8::Local<v8::Value> value);

v8::Local<v8::Integer> new_int32(v8::Isolate *isolate, int32_t value);
int32_t get_int32(v8::Local<v8::Value> value);

v8::Local<v8::Value> new_type_error(v8::Isolate *isolate, const std::string &message);
void throw_type_error(v8::Isolate *isolate, const std::string &message);
bool valid_arglen(const v8::FunctionCallbackInfo<v8::Value> &args, int expect,
                  const std::string &desc);
void get_exception(v8::Local<v8::Context> context, v8::TryCatch *try_catch, std::string *message,
                   std::string *stack);

v8::Local<v8::FunctionTemplate> new_function_template(v8::Isolate *isolate, v8::FunctionCallback fn,
                                                      void *data);
v8::Local<v8::Function> new_function(v8::Isolate *isolate, v8::FunctionCallback fn, void *data);

bool is_sequence(v8::Local<v8::Context> context, v8::Local<v8::Value> value);

v8::Local<v8::Promise::Resolver> promise_undefined(v8::Isolate *isolate);
v8::Local<v8::Promise::Resolver> promise_resolved(v8::Isolate *isolate, v8::Local<v8::Value> value);
v8::Local<v8::Promise> promise_then(v8::Isolate *isolate, v8::Local<v8::Promise> promise,
                                    v8::Local<v8::Function> onFulfilled,
                                    v8::Local<v8::Function> onRejected);
}  // namespace v8wrap
