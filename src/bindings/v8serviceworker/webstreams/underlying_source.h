/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>
#include <v8.h>

namespace v8serviceworker {

class UnderlyingSource : public common::HeapObject {
 public:
  v8::Eternal<v8::Function> cancelAlgorithm_;
  v8::Eternal<v8::Function> pullAlgorithm_;
  v8::Eternal<v8::Function> startAlgorithm_;
  v8::Eternal<v8::Function> sizeAlgorithm_;

 private:
  friend class common::Heap;
  UnderlyingSource() {}
  ~UnderlyingSource() {}
};

}  // namespace v8serviceworker