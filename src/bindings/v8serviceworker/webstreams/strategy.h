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
}
