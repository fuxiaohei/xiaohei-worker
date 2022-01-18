/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>

namespace v8serviceworker {

void register_webstreams_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder);

};
