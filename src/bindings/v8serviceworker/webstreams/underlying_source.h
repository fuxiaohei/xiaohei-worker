/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>
#include <v8.h>

#include <string>

namespace v8serviceworker {

class UnderlyingSource : public common::HeapObject {
 public:
  static UnderlyingSource* setup(v8::Local<v8::Object> object);

 public:
  v8::Local<v8::Promise> call_start(v8::Local<v8::Object> controller);
  v8::Local<v8::Promise> call_pull(v8::Local<v8::Object> controller);

 private:
  v8::Local<v8::Promise> call_algorithm(v8::Local<v8::Object> controller, const std::string& name);

 private:
  v8::Eternal<v8::Function> cancelAlgorithm_;
  v8::Eternal<v8::Function> pullAlgorithm_;
  v8::Eternal<v8::Function> startAlgorithm_;

 private:
  friend class common::Heap;
  UnderlyingSource() {}
  ~UnderlyingSource() {}
};

}  // namespace v8serviceworker
