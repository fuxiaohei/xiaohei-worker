/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>
#include <v8.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>

namespace v8serviceworker {

class QueueChunk : public common::HeapObject {
 public:
  void setValue(v8::Isolate* isolate, v8::Local<v8::Value> value, int64_t size);

 private:
  friend class common::Heap;
  QueueChunk() = default;
  ~QueueChunk();

 private:
  v8::Persistent<v8::Value> value_;
  int64_t size_ = 0;
};
}  // namespace v8serviceworker
