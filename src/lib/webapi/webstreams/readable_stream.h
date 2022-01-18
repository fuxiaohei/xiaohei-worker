/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>

#include <string>

namespace webapi {

class ReadableStream : public common::HeapObject {
 private:
  friend class common::Heap;
  ReadableStream() {}
  ~ReadableStream() {}

 private:
  std::string state_ = "readable";
  void *reader_ = nullptr;
  std::string storeError_ = "";
  bool disturbed_ = false;
};
}  // namespace webapi
