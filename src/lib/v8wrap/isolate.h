/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>

#include <map>
#include <string>

#define V8WRAP_ISOLATE_MEMORY_MIN (1024 * 1024 * 8)
#define V8WRAP_ISOLATE_MEMORY_DEFAULT (1024 * 1024 * 128)
#define V8WRAP_ISOLATE_DATA_INDEX 3

namespace v8wrap {

// --- allocator

class Allocator : public v8::ArrayBuffer::Allocator {
 public:
  explicit Allocator(size_t maxSize = 0);
  ~Allocator();

  void *Allocate(size_t length) override;
  void *AllocateUninitialized(size_t length) override;
  void Free(void *data, size_t length) override;

  size_t get_current_size() const { return m_current_alloc; }

 private:
  size_t m_current_alloc = 0;
  size_t m_total_alloc = 0;
  size_t m_max_size = 0;

  v8::ArrayBuffer::Allocator *m_allocator = nullptr;
};

// --- isolate

v8::Isolate *new_isolate(v8::ArrayBuffer::Allocator *allocator,
                         size_t memory_size = V8WRAP_ISOLATE_MEMORY_DEFAULT);

// --- isolateData

class IsolateData {
 public:
  static void Register(v8::Isolate *isolate);
  static void Release(v8::Isolate *isolate);
  static IsolateData *Get(v8::Isolate *isolate);

 public:
  void setClassTemplate(const std::string &name, v8::Local<v8::FunctionTemplate> fnTpl);
  v8::Local<v8::FunctionTemplate> getClassTemplate(const std::string &name);
  bool hasClassTemplate(const std::string &name);
  v8::Isolate *get_isolate() const { return isolate_; }

 private:
  explicit IsolateData(v8::Isolate *isolate);
  ~IsolateData() = default;

 private:
  v8::Isolate *isolate_ = nullptr;
  std::map<std::string, v8::Eternal<v8::FunctionTemplate>> function_templates_;
};

}  // namespace v8wrap