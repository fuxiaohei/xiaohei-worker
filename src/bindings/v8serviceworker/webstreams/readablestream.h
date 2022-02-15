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

#include <string>

namespace v8serviceworker {

class ReadableStreamDefaultController;
class ReadableStreamGenericReader;

enum ReadableStreamState {
  ReadableStreamState_Readable,
  ReadableStreamState_Closed,
  ReadableStreamState_Errored,
};

class ReadableStream : public common::HeapObject {
 public:
  size_t getDesiredSize();

 public:
  bool isLocked() { return reader_ != nullptr; }
  size_t getNumReadRequests();

  void setSizeAlgorithm(v8::Local<v8::Function> sizeAlgorithm) {
    sizeAlgorithm_.Set(sizeAlgorithm->GetIsolate(), sizeAlgorithm);
  }
  void setHighWaterMark(size_t highWaterMark) { highWaterMark_ = highWaterMark; }

 public:
  ReadableStreamState state_ = ReadableStreamState_Readable;
  ReadableStreamDefaultController *controller_ = nullptr;

 private:
  friend class common::Heap;
  ReadableStream() {}
  ~ReadableStream() {}

 private:
  size_t get_high_water_mark() { return 1; }
  size_t get_queue_total_size() { return 0; }

 private:
  ReadableStreamGenericReader *reader_ = nullptr;
  std::string storeError_ = "";
  bool disturbed_ = false;

  size_t highWaterMark_ = 1;
  v8::Eternal<v8::Function> sizeAlgorithm_;
};

v8::Local<v8::FunctionTemplate> create_readablestream_template(v8wrap::IsolateData *isolateData);

}  // namespace v8serviceworker
