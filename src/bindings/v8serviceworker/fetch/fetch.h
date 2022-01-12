/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>
#include <webapi/fetch/fetch_headers.h>

namespace v8serviceworker {

void register_fetch_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *class_builder);

webapi::FetchHeaders *init_headers(v8::Isolate *isolate, v8::Local<v8::Value> value);

};  // namespace v8serviceworker
