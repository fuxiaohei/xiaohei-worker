/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/webstreams/underlying_source.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

static void underlying_source_start(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto resolver = v8wrap::promise_undefined(args.GetIsolate());
  args.GetReturnValue().Set(resolver->GetPromise());
}

static void underlying_source_pull(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto resolver = v8wrap::promise_undefined(args.GetIsolate());
  args.GetReturnValue().Set(resolver->GetPromise());
}

static void underlying_source_cancel(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto resolver = v8wrap::promise_undefined(args.GetIsolate());
  args.GetReturnValue().Set(resolver->GetPromise());
}

};  // namespace v8serviceworker