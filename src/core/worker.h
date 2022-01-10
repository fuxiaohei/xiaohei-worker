/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <common/common.h>
#include <hv/HttpServer.h>
#include <runtime/runtime.h>

#include <string>

namespace xhworker {
namespace core {

struct Manifest {
  std::string key;
  std::string fpath;
  std::string content;
  std::string manifest_fpath;
  int type;
  int error_code;
};

class Worker : public common::RefCounted {
 public:
  static Manifest* ReadManifest(const std::string& dir, const std::string& key);
  static Worker* Create(const std::string& key);
  static void Destroy(Worker* worker);

 public:
  int handle_http_request(const HttpContextPtr& ctx);
  int get_error_code() const { return error_code_; }

 private:
  explicit Worker(Manifest* manifest);
  ~Worker();
  void destroy() override;

 private:
  int prepare_runtime();
  std::string error_response_body();

  xhworker::runtime::Runtime* runner_ = nullptr;

  int error_code_ = 0;
  std::string error_msg_;
  Manifest* manifest_ = nullptr;
};

}  // namespace core
}  // namespace xhworker