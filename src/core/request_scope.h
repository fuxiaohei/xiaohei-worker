/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>
#include <hv/HttpServer.h>
#include <webapi/fetch_event.h>

namespace core {

class RequestScope {
 public:
  webapi::FetchEvent* create_fetch_event();

 public:
  common::Heap* heap_ = nullptr;

 private:
  friend class Worker;
  explicit RequestScope(const HttpContextPtr& ctx);
  ~RequestScope();

 private:
  HttpContextPtr ctx_;
};

}  // namespace core
