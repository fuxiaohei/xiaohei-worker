/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>
#include <webapi/event_target.h>

namespace v8serviceworker {

class JsEventListener : public webapi::EventListener {
 public:
  v8::Isolate *isolate = nullptr;
  v8::Eternal<v8::Function> function;

  void call(webapi::Event *event);

 private:
  friend class common::Heap;
  JsEventListener() = default;
  ~JsEventListener() = default;
};

}  // namespace v8serviceworker
