/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/webstreams/stream_promise.h>

namespace v8serviceworker {

StreamPromise* StreamPromise::Create(v8::Isolate* isolate) { return nullptr; }

StreamPromise* StreamPromise::CreateResolved(v8::Isolate* isolate, v8::Local<v8::Value> value) {
  return nullptr;
}

StreamPromise* StreamPromise::CreateRejected(v8::Isolate* isolate, v8::Local<v8::Value> value) {
  return nullptr;
}

void StreamPromise::markAsHandled(v8::Isolate* isolate) {}
void StreamPromise::markAsSilent(v8::Isolate* isolate) {}

};  // namespace v8serviceworker
