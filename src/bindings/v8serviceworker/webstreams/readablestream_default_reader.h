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
class StreamPromise;

class ReadableStreamGenericReader {
 public:
  void initialize(v8::Local<v8::Context> context, ReadableStream *stream);
  size_t getReadRequestsSize() { return 0; }

  ReadableStream *own_stream_ = nullptr;
  StreamPromise *closed_promise_ = nullptr;
};

class ReadableStreamDefaultReader : public common::HeapObject, public ReadableStreamGenericReader {
 public:
  static v8::Local<v8::Object> Aqcuire(v8::Local<v8::Context> context, ReadableStream *stream);

 public:
  void setup(v8::Local<v8::Context> context, ReadableStream *stream);

 private:
  friend class common::Heap;
  ReadableStreamDefaultReader() {}
  ~ReadableStreamDefaultReader() {}
};

v8::Local<v8::FunctionTemplate> create_readablestream_default_reader_template(
    v8wrap::IsolateData *isolateData);

}  // namespace v8serviceworker
