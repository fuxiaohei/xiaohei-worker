/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <core/request_scope.h>

namespace core {

RequestScope::RequestScope(const HttpContextPtr& ctx) : ctx_(ctx) {
  heap_ = common::Heap::Create();
}

RequestScope::~RequestScope() {}

webapi::FetchEvent* RequestScope::create_fetch_event() {
  auto fetchEvent = heap_->alloc<webapi::FetchEvent>();
  fetchEvent->data = this;
  return fetchEvent;
}

}  // namespace core
