/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>

#include <string>

namespace v8wrap {

static const char *get_version() { return v8::V8::GetVersion(); }

// --- initialize

void init(const std::string &icu_file = "");
void shutdown();

// --- context

v8::Local<v8::Context> new_context(v8::Isolate *isolate);
v8::Local<v8::Context> new_context(v8::Isolate *isolate,
                                   v8::Local<v8::ObjectTemplate> global_template);

// -- run script

struct Result {
  v8::Local<v8::Value> value;
  bool is_exception = false;
  std::string exception_message;
  std::string exception_stack;
};

Result run_script(v8::Isolate *isolate, const std::string &script);
Result run_script(v8::Local<v8::Context> context, const std::string &script);

};  // namespace v8wrap
