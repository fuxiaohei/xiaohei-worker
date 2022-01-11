/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <webapi/event_target.h>
#include <webapi/global_scope.h>

namespace webapi {

ServiceWorkerGlobalScope::ServiceWorkerGlobalScope() {
  heap_ = common::Heap::Create();
  event_target_ = heap_->alloc<EventTarget>();
}

ServiceWorkerGlobalScope::~ServiceWorkerGlobalScope() {
  heap_->free();
  heap_ = nullptr;
  event_target_ = nullptr;
}

// --- GlobalScope static methods

ServiceWorkerGlobalScope *ServiceWorkerGlobalScope::create() {
  return new ServiceWorkerGlobalScope();
}

}  // namespace webapi
