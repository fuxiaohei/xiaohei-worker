/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>

#define CLASS_SERVICEWORKER_GLOBAL_SCOPE "ServiceWorkerGlobalScope"
#define CLASS_WORKER_GLOBAL_SCOPE "WorkerGlobalScope"

#define CLASS_EVENT_TARGET "EventTarget"
#define CLASS_FETCH_EVENT "FetchEvent"
#define CLASS_FETCH_REQUEST "Request"
#define CLASS_FETCH_RESPONSE "Response"

#define CLASS_READABLE_STREAM "ReadableStream"
#define CLASS_READABLE_STREAM_DEFAULT_CONTROLLER "ReadableStreamDefaultController"

namespace v8serviceworker {

v8::Local<v8::FunctionTemplate> create_service_worker_global_scope(v8::Isolate *isolate);

};  // namespace v8serviceworker
