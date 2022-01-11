/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/event/event_target.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <v8wrap/js_class.h>

namespace v8serviceworker {

static void event_target_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {}
static void event_target_js_addEventListener(const v8::FunctionCallbackInfo<v8::Value> &args) {}
static void event_target_js_removeEventListener(const v8::FunctionCallbackInfo<v8::Value> &args) {}
static void event_target_js_dispatchEvent(const v8::FunctionCallbackInfo<v8::Value> &args) {}

v8::Local<v8::FunctionTemplate> create_event_target_template(v8wrap::IsolateData *isolateData) {
  auto isolate = isolateData->get_isolate();

  v8wrap::ClassBuilder event_target_builder(isolate, CLASS_EVENT_TARGET);
  event_target_builder.setConstructor(event_target_js_constructor);
  event_target_builder.setMethod("addEventListener", event_target_js_addEventListener);
  event_target_builder.setMethod("removeEventListener", event_target_js_removeEventListener);
  event_target_builder.setMethod("dispatchEvent", event_target_js_dispatchEvent);
  auto event_target_template = event_target_builder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_EVENT_TARGET, event_target_template);

  return event_target_template;
}
}  // namespace v8serviceworker
