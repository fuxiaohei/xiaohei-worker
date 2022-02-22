/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/webstreams/stream_promise.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

StreamPromise::~StreamPromise() { value_.Reset(); }

StreamPromise* StreamPromise::Create(v8::Isolate* isolate) {
  auto resolver = v8::Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();

  auto sPromise = v8rt::allocObject<StreamPromise>(isolate);
  sPromise->setResolver(isolate, resolver);
  return sPromise;
}

StreamPromise* StreamPromise::CreateResolved(v8::Isolate* isolate, v8::Local<v8::Value> value) {
  auto resolver = v8wrap::promise_resolved(isolate, value);
  auto sPromise = v8rt::allocObject<StreamPromise>(isolate);
  sPromise->setResolver(isolate, resolver);
  return sPromise;
}

StreamPromise* StreamPromise::CreateRejected(v8::Isolate* isolate, v8::Local<v8::Value> value) {
  auto resolver = v8wrap::promise_rejected(isolate, value);
  auto sPromise = v8rt::allocObject<StreamPromise>(isolate);
  sPromise->setResolver(isolate, resolver);
  return sPromise;
}

void StreamPromise::markAsHandled(v8::Isolate* isolate) {
  auto resolver = value_.Get(isolate);
  if (resolver.IsEmpty()) {
    return;
  }
  resolver->GetPromise()->MarkAsHandled();
}

void StreamPromise::markAsSilent(v8::Isolate* isolate) {
  auto resolver = value_.Get(isolate);
  if (resolver.IsEmpty()) {
    return;
  }
  resolver->GetPromise()->MarkAsSilent();
}

v8::Local<v8::Promise> StreamPromise::getPromise(v8::Isolate* isolate) {
  return value_.Get(isolate)->GetPromise();
}

void StreamPromise::setRejected(v8::Isolate* isolate, v8::Local<v8::Value> value) {
  value_.Get(isolate)->Reject(isolate->GetCurrentContext(), value).ToChecked();
}

};  // namespace v8serviceworker
