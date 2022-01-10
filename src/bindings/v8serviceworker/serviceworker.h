/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>

#define CLASS_SERVICEWORKER_GLOBAL_SCOPE "ServiceWorkerGlobalScope"
#define CLASS_WORKER_GLOBAL_SCOPE "WorkerGlobalScope"

#define CLASS_EVENT_TARGET "EventTarget"

namespace v8serviceworker {

v8::Local<v8::FunctionTemplate> create_service_worker_global_scope(v8::Isolate *isolate);

};  // namespace v8serviceworker