/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/event/event_target.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>

namespace v8serviceworker {

v8::Local<v8::FunctionTemplate> create_service_worker_global_scope(v8::Isolate *isolate) {
  auto isolateData = v8wrap::IsolateData::Get(isolate);

  auto event_target_template = create_event_target_template(isolateData);

  v8wrap::ClassBuilder worker_scope_builder(isolate, CLASS_WORKER_GLOBAL_SCOPE);
  worker_scope_builder.setConstructor(nullptr);
  worker_scope_builder.inherit(event_target_template);
  auto worker_global_scope_template = worker_scope_builder.getClassTemplate();
  isolateData->setClassTemplate(CLASS_WORKER_GLOBAL_SCOPE, worker_global_scope_template);

  v8wrap::ClassBuilder builder(isolate, CLASS_SERVICEWORKER_GLOBAL_SCOPE);
  builder.setConstructor(nullptr);
  builder.setMethod(CLASS_EVENT_TARGET, event_target_template);
  builder.inherit(worker_global_scope_template);
  auto service_worker_global_scope_template = builder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_SERVICEWORKER_GLOBAL_SCOPE,
                                service_worker_global_scope_template);
  return service_worker_global_scope_template;
}

};  // namespace v8serviceworker
