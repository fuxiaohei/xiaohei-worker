/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>
#include <hv/HttpServer.h>

#include <string>

namespace webapi {

class FetchHeaders : public common::HeapObject {
 public:
  void set(const std::string &key, const std::string &value) { headers_[key] = value; }

  void append(const std::string &key, const std::string &value) {
    if (has(key)) {
      headers_[key] += ", " + value;
    } else {
      headers_[key] = value;
    }
  }

  bool has(const std::string &key) const { return headers_.find(key) != headers_.end(); }

  http_headers getAll() const { return headers_; }

 private:
  friend class common::Heap;
  FetchHeaders() = default;
  ~FetchHeaders() = default;

 private:
  http_headers headers_;
};

}  // namespace webapi
