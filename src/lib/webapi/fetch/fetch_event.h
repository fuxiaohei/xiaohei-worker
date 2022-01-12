/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <webapi/event_target.h>

#include <string>

namespace webapi {

class FetchEvent : public Event {
 public:
  std::string getType() const;

  void cancel();
  bool isCancelled() const;

 private:
  friend class common::Heap;
  FetchEvent() = default;
  ~FetchEvent() = default;

 private:
  bool is_cancelled_ = false;
};

}  // namespace webapi
