/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/webstreams/underlying_source.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>

#include <string>

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

v8::Local<v8::Function> get_algorithm(v8::Isolate *isolate, v8::Local<v8::Object> object,
                                      const std::string &name, v8::FunctionCallback callback) {
  if (v8wrap::has_property(object, name)) {
    auto value = v8wrap::get_property(object, name);
    if (value->IsFunction()) {
      return value.As<v8::Function>();
    }
  }
  return v8wrap::new_function(isolate, callback, nullptr);
}

// --- UnderlyingSource ---

v8::Local<v8::Promise> UnderlyingSource::call_start(v8::Local<v8::Object> controller) {
  auto isolate = controller->GetIsolate();
  auto context = isolate->GetCurrentContext();

  auto fn = startAlgorithm_.Get(isolate);

  // TODO(fxh): handle exception in start function

  v8::Local<v8::Value> args[1] = {controller};
  auto maybeResult = fn->Call(context, context->Global(), 1, args);
  if (maybeResult.IsEmpty()) {
    return v8wrap::promise_undefined(isolate)->GetPromise();
  }
  auto result = maybeResult.ToLocalChecked();
  if (result->IsPromise()) {
    return result.As<v8::Promise>();
  }

  return v8wrap::promise_resolved(isolate, result)->GetPromise();
}

UnderlyingSource *UnderlyingSource::setup(v8::Local<v8::Object> object) {
  printf("setupUnderlyingSource\n");
  auto isolate = object->GetIsolate();
  auto context = isolate->GetCurrentContext();
  auto source = v8rt::allocObject<UnderlyingSource>(isolate);
  source->startAlgorithm_.Set(isolate,
                              get_algorithm(isolate, object, "start", underlying_source_start));
  source->pullAlgorithm_.Set(isolate,
                             get_algorithm(isolate, object, "pull", underlying_source_pull));
  source->cancelAlgorithm_.Set(isolate,
                               get_algorithm(isolate, object, "cancel", underlying_source_cancel));
  /*source->sizeAlgorithm_.Set(isolate,
                             get_algorithm(isolate, object, "size", underlying_source_size));*/
  return source;
}

};  // namespace v8serviceworker
