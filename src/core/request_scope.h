/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/heap.h>
#include <hv/HttpServer.h>
#include <webapi/fetch/fetch_event.h>
#include <webapi/fetch/fetch_request.h>
#include <webapi/fetch/fetch_response.h>

namespace core {

class RequestScope {
 public:
  webapi::FetchEvent* create_fetch_event();
  webapi::FetchRequest* create_fetch_request();

  void set_response(webapi::FetchResponse* response) { response_ = response; }

  void set_error_msg(int errcode, const std::string& msg, const std::string& stack = "");
  bool is_error() const { return error_code_ != 0; }

  int handle_response();

  int destroy();

 public:
  common::Heap* heap_ = nullptr;

 private:
  friend class Worker;
  explicit RequestScope(const HttpContextPtr& ctx);
  ~RequestScope();

 private:
  std::string error_response_body();

 private:
  HttpContextPtr ctx_;

  int error_code_ = 0;
  std::string error_msg_;
  std::string error_stack;

  int response_status_code_ = HTTP_STATUS_OK;
  std::atomic<bool> is_response_sent_;
  webapi::FetchResponse* response_ = nullptr;
};

}  // namespace core
