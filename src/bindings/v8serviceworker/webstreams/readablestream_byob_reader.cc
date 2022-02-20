/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_byob_reader.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

// --- ReadableStreamBYOBReader ---
v8::Local<v8::Object> ReadableStreamBYOBReader::Aqcuire(v8::Local<v8::Context> context,
                                                        ReadableStream *stream) {
  printf("----------ReadableStreamBYOBReader::Aqcuire----------\n");
}

// --- ReadableStreamBYOBReader js methods----

v8::Local<v8::FunctionTemplate> create_readablestream_byob_reader_template(
    v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder rdBuilder(isolateData->get_isolate(), CLASS_READABLE_STREAM_BYOB_READER);
  rdBuilder.setConstructor(nullptr);
  auto tpl = rdBuilder.getClassTemplate();
  isolateData->setClassTemplate(CLASS_READABLE_STREAM_BYOB_READER, tpl);
  return tpl;
}

}  // namespace v8serviceworker
