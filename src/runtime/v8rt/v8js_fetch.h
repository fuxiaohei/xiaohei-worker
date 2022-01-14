/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/common.h>
#include <core/request_scope.h>
#include <hv/EventLoop.h>
#include <hv/HttpServer.h>
#include <hv/http_client.h>
#include <runtime/runtime.h>
#include <v8.h>

namespace v8rt {

class V8FetchContext : public runtime::FetchContext, common::RefCounted {
 public:
  static V8FetchContext* Create(core::RequestScope* req_scope,
                                v8::Local<v8::Promise::Resolver> resolver) {
    return new V8FetchContext(req_scope, resolver);
  }

 public:
  int get_id() override { return fetchReqID_; }
  void set_url(const std::string& url) override;
  void do_request() override;
  bool is_do_requested() override { return is_sent_.load(); }
  void terminate() override;
  void destroy() override;

 private:
  explicit V8FetchContext(core::RequestScope* req_scope, v8::Local<v8::Promise::Resolver> resolver);
  ~V8FetchContext() = default;

 private:
  void notify_request_done();
  void fulfill_promise();

 private:
  int fetchReqID_ = -1;
  core::RequestScope* reqScope_ = nullptr;

  v8::Isolate* isolate_ = nullptr;
  v8::Eternal<v8::Context> context_;
  v8::Eternal<v8::Promise::Resolver> promise_resolver_;

  HttpRequestPtr req_ = nullptr;
  HttpResponsePtr resp_ = nullptr;

  hv::HttpClient async_client_;
  std::atomic<bool> is_sent_ = {false};

  hv::EventLoop* mainloop_ = nullptr;
};

}  // namespace v8rt