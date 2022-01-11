/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <webapi/event_target.h>

namespace webapi {

EventTarget::EventTarget() {}

EventTarget::~EventTarget() {
  if (listeners_.empty()) {
    return;
  }
  auto empty = std::vector<ListenerItem>();
  listeners_.swap(empty);
}

void EventTarget::addListener(const std::string &name, EventListener *listener) {
  std::lock_guard<std::mutex> lck(lock_);
  ListenerItem item;
  item.type = name;
  item.listener = listener;
  item.is_removed = false;
  listeners_.push_back(item);
}

void EventTarget::removeListener(const std::string &name, EventListener *listener) {
  std::lock_guard<std::mutex> lck(lock_);
  for (auto it = listeners_.begin(); it != listeners_.end(); it++) {
    if (it->type == name && it->listener == listener) {
      it->is_removed = true;
    }
  }
}

bool EventTarget::hasListener(const std::string &name) {
  std::lock_guard<std::mutex> lck(lock_);
  for (auto it = listeners_.begin(); it != listeners_.end(); it++) {
    if (it->type == name) {
      return true;
    }
  }
  return false;
}

void EventTarget::dispatchEvent(Event *event) {
  std::lock_guard<std::mutex> lck(lock_);
  for (auto it = listeners_.begin(); it != listeners_.end(); it++) {
    if (it->type != event->getType() || it->is_removed) {
      continue;
    }
    it->listener->call(event);
    if (event->isCancelled()) {
      return;
    }
  }
}

}  // namespace webapi
