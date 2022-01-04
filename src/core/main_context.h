/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <hv/HttpServer.h>

#include <string>

namespace xhworker {
namespace core {

class MainContext {
 public:
  static int Run(int argc, char **argv);

 private:
  static MainContext *Shared(int argc, char **argv);

 private:
  MainContext(int argc, char **argv);
  ~MainContext();

  int run(int argc, char **argv);
  void exit();

  int parse_flags(int argc, char **argv);
  int pares_config();

  int http_handler(const HttpContextPtr &ctx);

 private:
  std::string config_file_ = "config.ini";
  std::string directory_ = "./examples";

  int server_port_ = 18888;
  int server_thread_num_ = 4;
  std::string server_addr_ = "127.0.0.1";
  hv::HttpServer server_;
};

}  // namespace core

}  // namespace xhworker
