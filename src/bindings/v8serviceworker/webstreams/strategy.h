/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>
#include <v8.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>

#include <string>

namespace v8serviceworker {

v8::Local<v8::FunctionTemplate> create_countqueuing_strategy_template(
    v8wrap::IsolateData *isolateData);

v8::Local<v8::Object> create_countqueuing_strategy_instance(v8::Local<v8::Context> context,
                                                            v8::Local<v8::Value>);

v8::Local<v8::Function> make_countqueuing_strategy_size_algorithm(v8::Local<v8::Object> object);

int64_t make_countqueuing_strategy_high_water_mark(v8::Local<v8::Object> object,
                                                   int64_t defaultValue);

int64_t make_bytelengthqueuing_strategy_high_water_mark(v8::Local<v8::Object> object,
                                                        int64_t defaultValue);

}  // namespace v8serviceworker
