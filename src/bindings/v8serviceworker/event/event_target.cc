/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/event/event_listener.h>
#include <bindings/v8serviceworker/event/event_target.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <hv/hlog.h>
#include <runtime/v8rt/v8js_context.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>
#include <v8wrap/js_value.h>
#include <webapi/event_target.h>
#include <webapi/global_scope.h>

namespace v8serviceworker {

static webapi::EventTarget *get_event_target(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Local<v8::Object> this_object = args.Holder();

  auto context = args.GetIsolate()->GetCurrentContext();

  // global.addEventListener is defined on ServiceWorkerGlobalScope
  if (v8wrap::IsolateData::IsInstanceOf(context, this_object, CLASS_SERVICEWORKER_GLOBAL_SCOPE)) {
    auto global_scope =
        v8wrap::get_ptr<webapi::ServiceWorkerGlobalScope>(context, V8_JS_CONTEXT_JSGLOBAL_INDEX);
    if (global_scope == nullptr) {
      return nullptr;
    }
    return global_scope->event_target_;
  }

  // new EventTarget().addEventListener is defined on EventTarget
  if (v8wrap::IsolateData::IsInstanceOf(context, this_object, CLASS_EVENT_TARGET)) {
    return v8wrap::get_ptr<webapi::EventTarget>(this_object);
  }
  return nullptr;
}

static void event_target_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {}
static void event_target_js_addEventListener(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (v8wrap::valid_arglen(args, 2, "addEventListener: ")) {
    return;
  }
  auto isolate = args.GetIsolate();
  if (!args[1]->IsFunction()) {
    v8wrap::throw_type_error(isolate, "addEventListener: second argument must be a function");
    return;
  }
  auto eventTarget = get_event_target(args);
  if (eventTarget == nullptr) {
    v8wrap::throw_type_error(isolate, "addEventListener: EventTarget is null");
    return;
  }

  // create a new JsEventListener
  std::string event_name = v8wrap::to_string(isolate->GetCurrentContext(), args[0]);

  auto listener = xhworker::v8rt::allocObject<JsEventListener>(isolate);
  listener->function.Set(isolate, args[1].As<v8::Function>());
  listener->isolate = isolate;
  eventTarget->addListener(event_name, listener);
  hlogd("event_target_js_addEventListener, event_name:%s, listener:%p , eventTarget:%p",
        event_name.c_str(), listener, eventTarget);
}
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
