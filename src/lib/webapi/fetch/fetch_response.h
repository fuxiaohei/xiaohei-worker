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

class FetchHeaders;

class FetchResponse : public common::HeapObject {
 public:
  HttpResponse *getRawResponse() { return response_; }

  void setBody(const std::string &body) { response_->SetBody(body); }

  void setStatus(int status) { response_->status_code = http_status(status); }

  void setHeaders(FetchHeaders *headers);
  void setHeader(const std::string &key, const std::string &value);

 private:
  friend class common::Heap;
  FetchResponse() { response_ = new HttpResponse(); }
  ~FetchResponse() { delete response_; }

 private:
  HttpResponse *response_ = nullptr;
};
}  // namespace webapi
