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

#include <vector>

namespace v8serviceworker {
class ReadableStream;

void setupReadableByteStreamControllerFromSource(const v8::FunctionCallbackInfo<v8::Value> &args,
                                                    ReadableStream *rs);
}  // namespace v8serviceworker
