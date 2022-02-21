/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_default_controller.h>
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
      return;
      // 4. Otherwise, if stream.[[state]] is "closed",
    case ReadableStreamState_Closed:
      closed_promise_ = StreamPromise::CreateResolved(isolate, v8::Undefined(isolate));
      return;
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
  auto isolate = context->GetIsolate();
  auto reader = v8rt::allocObject<ReadableStreamDefaultReader>(isolate);
  v8::Local<v8::Value> readerObj;
  if (!v8wrap::IsolateData::NewInstance(context, CLASS_READABLE_STREAM_DEFAULT_READER, &readerObj,
                                        reader)) {
    v8wrap::throw_type_error(isolate, "create ReadableStreamDefaultReader failed");
    return v8::Local<v8::Object>();
  }
  reader->setup(context, stream);
  hlogd("ReadableStreamDefaultReader: aqcuire %p", reader);
  v8wrap::set_ptr(readerObj.As<v8::Object>(), reader);
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

v8::Local<v8::Promise> ReadableStreamDefaultReader::read(v8::Local<v8::Context> context) {
  auto isolate = context->GetIsolate();
  // https://streams.spec.whatwg.org/#readable-stream-default-reader-read
  // 1. Let stream be reader.[[ownerReadableStream]].
  // 2. Assert: stream is not undefined.
  // 3. Set stream.[[disturbed]] to true.
  own_stream_->is_disturbed_ = true;
  StreamPromise *promise = nullptr;
  switch (own_stream_->state_) {
    // 4. If stream.[[state]] is "closed", return a promise resolved with !
    //    ReadableStreamCreateReadResult(undefined, true,
    //    reader.[[forAuthorCode]]).
    case ReadableStreamState_Closed:
      promise = StreamPromise::CreateResolved(
          isolate, ReadableStream::CreateReadResult(isolate, v8::Undefined(isolate), true));
      return promise->getPromise(isolate);
    // 5. If stream.[[state]] is "errored", return a promise rejected with
    //    stream.[[storedError]].
    case ReadableStreamState_Errored:
      promise = StreamPromise::CreateRejected(isolate, own_stream_->getError(isolate));
      return promise->getPromise(isolate);
    case ReadableStreamState_Readable:
      // 6. Assert: stream.[[state]] is "readable".
      // 7. Return ! stream.[[readableStreamController]].[[PullSteps]]().
      return own_stream_->controller_->pullSteps(context);
  }
}

// --- ReadableStreamDefaultReader js methods----

static void default_reader_js_closed_getter(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void default_reader_js_cancel(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void default_reader_js_read(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto isolate = args.GetIsolate();
  auto reader = v8wrap::get_ptr<ReadableStreamDefaultReader>(args.Holder());

  // https://streams.spec.whatwg.org/#default-reader-read
  // 2. If this.[[ownerReadableStream]] is undefined, return a promise rejected
  //  with a TypeError exception
  if (reader->own_stream_ == nullptr) {
    auto value =
        v8wrap::new_type_error(args.GetIsolate(), "ReadableStreamDefaultReader is released");
    args.GetReturnValue().Set(StreamPromise::CreateRejected(isolate, value)->getPromise(isolate));
    return;
  }

  // 3. Return ! ReadableStreamReaderRead(this).
  args.GetReturnValue().Set(reader->read(isolate->GetCurrentContext()));
}

static void default_reader_js_releaseLock(const v8::FunctionCallbackInfo<v8::Value> &args) {}

v8::Local<v8::FunctionTemplate> create_readablestream_default_reader_template(
    v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder rdBuilder(isolateData->get_isolate(), CLASS_READABLE_STREAM_DEFAULT_READER);
  rdBuilder.setConstructor(nullptr);
  rdBuilder.setAccessorProperty("closed", default_reader_js_closed_getter, nullptr);
  rdBuilder.setMethod("cancel", default_reader_js_cancel);
  rdBuilder.setMethod("read", default_reader_js_read);
  rdBuilder.setMethod("releaseLock", default_reader_js_releaseLock);
  auto tpl = rdBuilder.getClassTemplate();
  isolateData->setClassTemplate(CLASS_READABLE_STREAM_DEFAULT_READER, tpl);
  return tpl;
}

}  // namespace v8serviceworker
