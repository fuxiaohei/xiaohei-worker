/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/timer.h>

#include <string>

namespace v8serviceworker {

static void js_setTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void js_clearTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void js_setInterval(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void js_clearInterval(const v8::FunctionCallbackInfo<v8::Value> &args) {}

void register_timer_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  classBuilder->setMethod("setTimeout", js_setTimeout);
  classBuilder->setMethod("clearTimeout", js_clearTimeout);
  classBuilder->setMethod("setInterval", js_setInterval);
  classBuilder->setMethod("clearInterval", js_clearInterval);
}

};  // namespace v8serviceworker
