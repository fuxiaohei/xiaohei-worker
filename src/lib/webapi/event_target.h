/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>

#include <mutex>
#include <string>
#include <vector>

namespace webapi {

class Event;

class EventListener : public common::HeapObject {
 public:
  virtual void call(Event *event) = 0;
};

class Event : public common::HeapObject {
 public:
  virtual std::string getType() const = 0;

  virtual void cancel() = 0;
  virtual bool isCancelled() const = 0;

  void *data;
};

class EventTarget : public common::HeapObject {
 public:
  EventTarget();
  ~EventTarget();

  void addListener(const std::string &name, EventListener *listener);
  void removeListener(const std::string &name, EventListener *listener);
  void dispatchEvent(Event *event);
  bool hasListener(const std::string &name);

 private:
  struct ListenerItem {
    std::string type;
    EventListener *listener;
    bool is_removed;
  };

  std::vector<ListenerItem> listeners_;
  std::mutex lock_;
};

}  // namespace webapi
