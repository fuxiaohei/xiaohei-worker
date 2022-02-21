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

class StreamPromise : public common::HeapObject {
 public:
  static StreamPromise* Create(v8::Isolate* isolate);
  static StreamPromise* CreateResolved(v8::Isolate* isolate, v8::Local<v8::Value> value);
  static StreamPromise* CreateRejected(v8::Isolate* isolate, v8::Local<v8::Value> value);

 public:
  void markAsHandled(v8::Isolate* isolate);
  void markAsSilent(v8::Isolate* isolate);

  v8::Local<v8::Promise> getPromise(v8::Isolate* isolate);
  void setResolver(v8::Isolate* isolate, v8::Local<v8::Promise::Resolver> resolver) {
    value_.Reset(isolate, resolver);
  }

 private:
  friend class common::Heap;
  StreamPromise() = default;
  ~StreamPromise();

 private:
  v8::Persistent<v8::Promise::Resolver> value_;
};

}  // namespace v8serviceworker
