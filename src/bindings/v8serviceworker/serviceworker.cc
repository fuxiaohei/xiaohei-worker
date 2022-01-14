/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/base64/base64.h>
#include <bindings/v8serviceworker/event/event_target.h>
#include <bindings/v8serviceworker/event/fetch_event.h>
#include <bindings/v8serviceworker/fetch/fetch.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>

namespace v8serviceworker {

v8::Local<v8::FunctionTemplate> create_service_worker_global_scope(v8::Isolate *isolate) {
  auto isolateData = v8wrap::IsolateData::Get(isolate);

  auto eventTargetTemplate = create_event_target_template(isolateData);

  v8wrap::ClassBuilder workerScopeBuilder(isolate, CLASS_WORKER_GLOBAL_SCOPE);
  workerScopeBuilder.setConstructor(nullptr);
  workerScopeBuilder.inherit(eventTargetTemplate);
  auto workerScopeTemplate = workerScopeBuilder.getClassTemplate();
  isolateData->setClassTemplate(CLASS_WORKER_GLOBAL_SCOPE, workerScopeTemplate);

  v8wrap::ClassBuilder builder(isolate, CLASS_SERVICEWORKER_GLOBAL_SCOPE);
  builder.setConstructor(nullptr);
  builder.setMethod(CLASS_EVENT_TARGET, eventTargetTemplate);
  builder.inherit(workerScopeTemplate);
  auto globalScopeTemplate = builder.getClassTemplate();

  register_fetch_event(isolateData, &builder);
  register_fetch_api(isolateData, &builder);
  register_base64_api(isolateData, &builder);

  isolateData->setClassTemplate(CLASS_SERVICEWORKER_GLOBAL_SCOPE, globalScopeTemplate);
  return globalScopeTemplate;
}

};  // namespace v8serviceworker
