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

class ReadableStream;
class UnderlyingSource;

class ReadableStreamDefaultController : public common::HeapObject {
 public:
  size_t get_queue_total_size();
  size_t getDesiredSize() {
    if (stream_ == nullptr) return 0;
    return stream_->getDesiredSize();
  }

 public:
  bool closeRequested_ = false;
  bool pull_again_ = false;
  bool pulling_ = false;
  bool started_ = false;

  bool is_close_requested_ = false;

  ReadableStream *stream_ = nullptr;
  UnderlyingSource *source_ = nullptr;

  v8::Eternal<v8::Object> js_object_;

 private:
  friend class common::Heap;
  ReadableStreamDefaultController() {}
  ~ReadableStreamDefaultController() {}

 private:
  std::vector<int> queue_;
};

v8::Local<v8::FunctionTemplate> create_readablestream_controller_template(
    v8wrap::IsolateData *isolateData);

void setupReadableStreamDefaultControllerFromSource(const v8::FunctionCallbackInfo<v8::Value> &args,
                                                    ReadableStream *rs);

}  // namespace v8serviceworker
