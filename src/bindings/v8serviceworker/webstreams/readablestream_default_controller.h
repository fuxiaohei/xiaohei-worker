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

#include <queue>

namespace v8serviceworker {

class ReadableStream;
class UnderlyingSource;
class QueueChunk;

class ReadableStreamDefaultController : public common::HeapObject {
 public:
  v8::Local<v8::Promise> pullSteps(v8::Local<v8::Context> context);
  v8::Local<v8::Promise> cancelSteps(v8::Local<v8::Context> context,
                                     v8::Local<v8::Value> reason);

 public:
  size_t get_queue_total_size();
  size_t getDesiredSize() {
    if (stream_ == nullptr) return 0;
    return stream_->getDesiredSize();
  }
  void enqueue(v8::Isolate *isolate, v8::Local<v8::Value> value, int64_t size);
  void resetQueue();
  bool isQueueEmpty() { return queue_.size() == 0; }
  void clearAlgorithms();

 public:
  bool closeRequested_ = false;
  bool pull_again_ = false;
  bool pulling_ = false;
  bool started_ = false;

  bool is_close_requested_ = false;

  ReadableStream *stream_ = nullptr;
  UnderlyingSource *source_ = nullptr;

  v8::Eternal<v8::Object> js_object_;

  int64_t queue_total_size_ = 0;

 private:
  friend class common::Heap;
  ReadableStreamDefaultController() {}
  ~ReadableStreamDefaultController() {}

 private:
  QueueChunk *dequeue();

 private:
  std::queue<QueueChunk *> queue_;
};

v8::Local<v8::FunctionTemplate> create_readablestream_controller_template(
    v8wrap::IsolateData *isolateData);

void setupReadableStreamDefaultControllerFromSource(const v8::FunctionCallbackInfo<v8::Value> &args,
                                                    ReadableStream *rs);

bool readableStreamDefaultControllerCanCloseOrEnqueue(ReadableStreamDefaultController *controller);
void readableStreamDefaultControllerCallPullIfNeeded(v8::Isolate *isolate,
                                                     ReadableStreamDefaultController *controller);
bool readableStreamDefaultControllerShouldCallPull(ReadableStreamDefaultController *controller);
void readableStreamDefaultControllerError(ReadableStreamDefaultController *controller,
                                          v8::Local<v8::Value> error);

void readableStreamDefaultControllerEnqueueValueWithSize(
    const v8::FunctionCallbackInfo<v8::Value> &args, ReadableStreamDefaultController *controller,
    v8::Local<v8::Value> chunk, int64_t chunkSize);

}  // namespace v8serviceworker
