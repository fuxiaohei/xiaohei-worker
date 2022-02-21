/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/webstreams/queue_chunk.h>

namespace v8serviceworker {

void QueueChunk::setValue(v8::Isolate* isolate, v8::Local<v8::Value> value, int64_t size) {
  value_.Reset(isolate, value);
  size_ = size;
}

v8::Local<v8::Value> QueueChunk::getValue(v8::Isolate* isolate) { return value_.Get(isolate); }

QueueChunk::~QueueChunk() { value_.Reset(); }

}  // namespace v8serviceworker
