/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <lib/v8wrap/isolate.h>

namespace v8wrap {
v8::Isolate *new_isolate(v8::ArrayBuffer::Allocator *allocator, size_t memory_size) {
  v8::ResourceConstraints rc;
  rc.ConfigureDefaultsFromHeapSize(memory_size / 10, memory_size);
  v8::Isolate::CreateParams params;
  params.array_buffer_allocator = allocator;
  params.constraints = rc;
  auto isolate = v8::Isolate::New(params);
  IsolateData::Register(isolate);
  return isolate;
}

// -- Allocator

Allocator::Allocator(size_t maxSize) : m_max_size(maxSize) {
  m_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
}

Allocator::~Allocator() { delete m_allocator; }

void *Allocator::Allocate(size_t length) {
  m_total_alloc += length;
  m_current_alloc += length;
  return m_allocator->Allocate(length);
}

void *Allocator::AllocateUninitialized(size_t length) {
  m_total_alloc += length;
  m_current_alloc += length;
  return m_allocator->AllocateUninitialized(length);
}

void Allocator::Free(void *data, size_t length) {
  m_current_alloc -= length;
  m_allocator->Free(data, length);
}

// --- IsolateData

void IsolateData::Register(v8::Isolate *isolate) { new IsolateData(isolate); }

void IsolateData::Release(v8::Isolate *isolate) {
  auto isolateData = Get(isolate);
  if (isolateData) {
    delete isolateData;
  }
}

IsolateData *IsolateData::Get(v8::Isolate *isolate) {
  return static_cast<IsolateData *>(isolate->GetData(V8WRAP_ISOLATE_DATA_INDEX));
}

IsolateData::IsolateData(v8::Isolate *isolate) : isolate_(isolate) {
  isolate_->SetData(V8WRAP_ISOLATE_DATA_INDEX, this);
}

void IsolateData::setClassTemplate(const std::string &name, v8::Local<v8::FunctionTemplate> fnTpl) {
  if (hasClassTemplate(name)) {
    return;
  }
  function_templates_[name] = v8::Eternal<v8::FunctionTemplate>(isolate_, fnTpl);
}

v8::Local<v8::FunctionTemplate> IsolateData::getClassTemplate(const std::string &name) {
  auto it = function_templates_.find(name);
  if (it == function_templates_.end()) {
    return v8::Local<v8::FunctionTemplate>();
  }
  return it->second.Get(isolate_);
}

bool IsolateData::hasClassTemplate(const std::string &name) {
  auto it = function_templates_.find(name);
  return it != function_templates_.end();
}

}  // namespace v8wrap
