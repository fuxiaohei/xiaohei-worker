/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_default_reader.h>
#include <bindings/v8serviceworker/webstreams/stream_promise.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

// --- ReadableStreamGenericReader ---

void ReadableStreamGenericReader::initialize(v8::Local<v8::Context> context,
                                             ReadableStream *stream) {
  // https://streams.spec.whatwg.org/#readable-stream-reader-generic-initialize
  // 1. Set reader.[[ownerReadableStream]] to stream.
  own_stream_ = stream;
  // 2. Set stream.[[reader]] to reader.
  stream->setReader(this);

  auto isolate = context->GetIsolate();
  switch (stream->state_) {
    // 3. If stream.[[state]] is "readable",
    case ReadableStreamState_Readable:
      closed_promise_ = StreamPromise::Create(isolate);
      // 4. Otherwise, if stream.[[state]] is "closed",
    case ReadableStreamState_Closed:
      closed_promise_ = StreamPromise::CreateResolved(isolate, v8::Undefined(isolate));
      // 5. Otherwise,
    case ReadableStreamState_Errored:
      //    b. Set reader.[[closedPromise]] to a promise rejected with stream.
      //    [[storedError]].
      closed_promise_ = StreamPromise::CreateRejected(isolate, stream->getError(isolate));
      closed_promise_->markAsSilent(isolate);
      //    c. Set reader.[[closedPromise]].[[PromiseIsHandled]] to true.
      closed_promise_->markAsHandled(isolate);
  }
}

// --- ReadableStreamDefaultReader ---

v8::Local<v8::Object> ReadableStreamDefaultReader::Aqcuire(v8::Local<v8::Context> context,
                                                           ReadableStream *stream) {
  printf("----------ReadableStreamDefaultReader::Aqcuire\n");

  auto isolate = context->GetIsolate();
  auto reader = v8rt::allocObject<ReadableStreamDefaultReader>(isolate);
  v8::Local<v8::Value> readerObj;
  if (!v8wrap::IsolateData::NewInstance(context, CLASS_READABLE_STREAM_DEFAULT_READER, &readerObj,
                                        reader)) {
    v8wrap::throw_type_error(isolate, "create ReadableStreamDefaultReader failed");
    return v8::Local<v8::Object>();
  }
  reader->setup(context, stream);
  return readerObj.As<v8::Object>();
}

void ReadableStreamDefaultReader::setup(v8::Local<v8::Context> context, ReadableStream *stream) {
  // https://streams.spec.whatwg.org/#set-up-readable-stream-default-reader
  // 1. If ! IsReadableStreamLocked(stream) is true, throw a TypeError
  //    exception.
  if (stream->isLocked()) {
    v8wrap::throw_type_error(context->GetIsolate(), "ReadableStream is locked");
    return;
  }

  // 2. Perform ! ReadableStreamReaderGenericInitialize(reader, stream).
  initialize(context, stream);
  // 3. Set reader.[[readRequests]] to a new empty List.
}

// --- ReadableStreamDefaultReader js methods----

v8::Local<v8::FunctionTemplate> create_readablestream_default_reader_template(
    v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder rdBuilder(isolateData->get_isolate(), CLASS_READABLE_STREAM_DEFAULT_READER);
  rdBuilder.setConstructor(nullptr);
  auto tpl = rdBuilder.getClassTemplate();
  isolateData->setClassTemplate(CLASS_READABLE_STREAM_DEFAULT_READER, tpl);
  return tpl;
}

}  // namespace v8serviceworker
