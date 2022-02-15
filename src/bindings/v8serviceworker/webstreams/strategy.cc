/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/strategy.h>
#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

static void ountqueuing_strategy_js_constructor(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (args.Length() > 0 && args[0]->IsObject()) {
    auto options = args[0].As<v8::Object>();
    if (v8wrap::has_property(options, "highWaterMark")) {
      auto value = v8wrap::get_property(options, "highWaterMark");
      if (!value->IsNumber()) {
        v8wrap::throw_range_error(args.GetIsolate(), "highWaterMark must be a number");
        return;
      }
      auto highWaterMark = value->Int32Value(args.GetIsolate()->GetCurrentContext()).ToChecked();
      v8wrap::set_property(args.Holder(), "highWaterMark",
                           v8wrap::new_int32(args.GetIsolate(), highWaterMark));
    }
  }
}

static void countqueuing_strategy_js_size(const v8::FunctionCallbackInfo<v8::Value> &args) {
  args.GetReturnValue().Set(1);
}

v8::Local<v8::FunctionTemplate> create_countqueuing_strategy_template(
    v8wrap::IsolateData *isolateData) {
  v8wrap::ClassBuilder strategyBuilder(isolateData->get_isolate(), CLASS_COUNT_QUEUING_STRATEGY);
  strategyBuilder.setConstructor(ountqueuing_strategy_js_constructor);
  strategyBuilder.setMethod("size", countqueuing_strategy_js_size);
  strategyBuilder.setValue("highWaterMark", v8wrap::new_int32(isolateData->get_isolate(), 1));
  auto strategyTemplate = strategyBuilder.getClassTemplate();
  isolateData->setClassTemplate(CLASS_COUNT_QUEUING_STRATEGY, strategyTemplate);
  return strategyTemplate;
}

v8::Local<v8::Object> create_countqueuing_strategy_instance(v8::Local<v8::Context> context) {
  v8::Local<v8::Value> strategy_obj;
  if (!v8wrap::IsolateData::NewInstance(context, CLASS_READABLE_STREAM_DEFAULT_CONTROLLER,
                                        &strategy_obj, nullptr)) {
    v8wrap::throw_type_error(context->GetIsolate(), "create CountQueuingStrategy object failed");
    return v8::Local<v8::Object>();
  }
  return strategy_obj.As<v8::Object>();
}

v8::Local<v8::Function> make_countqueuing_strategy_size_algorithm(v8::Local<v8::Object> object) {
  if (v8wrap::has_property(object, "size")) {
    auto sizeValue = v8wrap::get_property(object, "size");
    if (!sizeValue->IsFunction()) {
      v8wrap::throw_type_error(object->GetIsolate(), "size must be a function");
      return v8::Local<v8::Function>();
    }
    return sizeValue.As<v8::Function>();
  }
  return v8wrap::new_function(object->GetIsolate(), countqueuing_strategy_js_size, nullptr);
}

size_t make_countqueuing_strategy_high_water_mark(v8::Local<v8::Object> object,
                                                  size_t defaultValue) {
  if (v8wrap::has_property(object, "highWaterMark")) {
    auto value = v8wrap::get_property(object, "highWaterMark");
    if (value->IsUndefined()) {
      return defaultValue;
    }
    return v8wrap::get_int32(value);
  }
  return defaultValue;
}

size_t make_bytelengthqueuing_strategy_high_water_mark(v8::Local<v8::Object> object,
                                                       size_t defaultValue) {
  return make_countqueuing_strategy_high_water_mark(object, defaultValue);
}

}  // namespace v8serviceworker
