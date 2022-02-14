/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_controller.h>
#include <bindings/v8serviceworker/webstreams/readablestream_reader.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

// --- ReadableStream ---

size_t ReadableStream::getDesiredSize() { return get_high_water_mark() - get_queue_total_size(); }

size_t ReadableStream::getNumReadRequests() {
  printf("ReadableStream::getNumReadRequests():%p\n", reader_);
  assert(reader_ != nullptr);
  return reader_->getReadRequestsSize();
}

// --- ReadableStream Js Methods --

static void readablestream_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (args.Length() == 1) {
    if (!args[0]->IsObject()) {
      v8wrap::throw_type_error(args.GetIsolate(),
                               "first argument must be an UnderlyingSource object");
      return;
    }
  }

  // use ReadableStream to save state
  auto isolate = args.GetIsolate();
  auto stream = v8rt::allocObject<ReadableStream>(isolate);

  // setup controller
  setupReadableStreamDefaultControllerFromSource(args, stream);
}

v8::Local<v8::FunctionTemplate> create_readablestream_template(v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder rsBuilder(isolateData->get_isolate(), CLASS_READABLE_STREAM);
  rsBuilder.setConstructor(readablestream_js_constructor);
  auto rsTemplate = rsBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_READABLE_STREAM, rsTemplate);

  return rsTemplate;
}

};  // namespace v8serviceworker
