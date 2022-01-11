/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>

namespace webapi {

class EventTarget;

class ServiceWorkerGlobalScope {
 public:
  static ServiceWorkerGlobalScope *create();

 private:
  ServiceWorkerGlobalScope();
  ~ServiceWorkerGlobalScope();

 public:
  common::Heap *heap_ = nullptr;
  EventTarget *event_target_ = nullptr;
};

}  // namespace webapi
