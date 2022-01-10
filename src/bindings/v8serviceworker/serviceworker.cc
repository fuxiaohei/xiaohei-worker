/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_class.h>

namespace v8serviceworker {

v8::Local<v8::FunctionTemplate> create_service_worker_global_scope(v8::Isolate *isolate) {
  auto isolateData = v8wrap::IsolateData::Get(isolate);

  v8wrap::ClassBuilder builder(isolate, CLASS_SERVICEWORKER_GLOBAL_SCOPE);
  builder.setConstructor(nullptr);

  auto service_worker_global_scope_template = builder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_SERVICEWORKER_GLOBAL_SCOPE,
                                service_worker_global_scope_template);
  return service_worker_global_scope_template;
}

};  // namespace v8serviceworker