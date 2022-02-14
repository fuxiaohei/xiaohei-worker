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

#include <vector>

namespace v8serviceworker {

class ReadableStreamGenericReader {
 public:
  size_t getReadRequestsSize() { return 1; }
};

class ReadableStreamDefaultReader : public common::HeapObject, public ReadableStreamGenericReader {
 private:
  friend class common::Heap;
  ReadableStreamDefaultReader() {}
  ~ReadableStreamDefaultReader() {}
};

}  // namespace v8serviceworker
