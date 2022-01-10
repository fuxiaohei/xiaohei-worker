/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <core/container.h>
#include <core/main_context.h>
#include <core/worker.h>
#include <hv/hlog.h>
#include <hv/hmain.h>
#include <hv/hthread.h>
#include <hv/hversion.h>
#include <hv/iniparser.h>
#include <lib/v8wrap/v8wrap.h>
#include <stdio.h>

#include <cstdlib>

namespace xhworker {
namespace core {

MainContext::MainContext(int argc, char **argv) {
  // set ctrl+c handler
  auto sys_signal_handler = [](int signum) {
    if (signum == SIGINT) {
      hlogi("http server stop");
      Shared(0, nullptr)->exit();
      v8wrap::shutdown();
      hlogi("process exit, pid=%d", hv_getpid());
      std::exit(0);
      return;
    }
    std::exit(signum);
  };

  signal(SIGINT, sys_signal_handler);
}

MainContext::~MainContext() {}

MainContext *MainContext::Shared(int argc, char **argv) {
  static MainContext *instance = new MainContext(argc, argv);
  return instance;
}

int MainContext::Run(int argc, char **argv) {
  auto *mainContext = Shared(argc, argv);
  return mainContext->run(argc, argv);
}

int MainContext::run(int argc, char **argv) {
  // parse flags and configs
  if (parse_flags(argc, argv) != 0) {
    return -1;
  }

  // init v8
  v8wrap::init("");

  // set http router
  hv::HttpService service;
  service.GET("*", std::bind(&MainContext::http_handler, this, std::placeholders::_1));

  // start http server
  server_.setPort(server_port_);
  server_.setHost(server_addr_.c_str());
  server_.setThreadNum(server_thread_num_);
  server_.registerHttpService(&service);
  return server_.run();
}

int MainContext::http_handler(const HttpContextPtr &ctx) {
  // generate key by path
  auto key = ctx->request->Path();
  key.erase(key.begin());
  if (key.back() == '/') {
    key.pop_back();
  }

  // get worker
  auto worker = Container::GetWorker(key);
  if (worker == nullptr) {
    ctx->setStatus(HTTP_STATUS_NOT_FOUND);
    return ctx->sendString("script not found");
  }

  return worker->handle_http_request(ctx);
}

void MainContext::exit() { server_.stop(); }

int MainContext::parse_flags(int argc, char **argv) {
  // long options
  static option_t long_options[] = {{'h', "help", NO_ARGUMENT},
                                    {'v', "version", NO_ARGUMENT},
                                    {'c', "configfile", REQUIRED_ARGUMENT}};

  auto print_version = []() {
    printf("%s %s (%s)\nlibhv %s\nv8 %s\n", "worker", "0.1.0", "alpha", hv_compile_version(),
           "???");
  };
  auto print_help = []() { printf("help ???\n"); };

  // handle command line options
  int ret = parse_opt_long(argc, argv, long_options, ARRAY_SIZE(long_options));
  if (ret != 0) {
    print_help();
    return ret;
  }

  // help
  if (get_arg("h")) {
    print_help();
    return 0;
  }

  // version
  if (get_arg("v")) {
    print_version();
    return 0;
  }

  // config file
  const char *config_file = get_arg("c");
  if (config_file) {
    config_file_ = config_file;
  } else {
    config_file_ = "./config.ini";
  }
  return pares_config();
}

int MainContext::pares_config() {
  IniParser ini;
  int ret = ini.LoadFromFile(config_file_.data());
  if (ret != 0) {
    hlogf("config load fail, file:%s, ret:%d", config_file_.data(), ret);
    return -40;
  }

  // set port
  auto port = ini.Get<int>("http_server_port");
  if (port < 1) {
    hlogf("config load port fail, file:%s, port:%d", config_file_.data(), port);
    return -40;
  }
  hlogd("config load port, port:%d", port);
  server_port_ = port;

  // set thread num
  auto thread_num = ini.Get<int>("http_server_thread_num");
  if (thread_num < 1) {
    hlogf("config load thread_num fail, file:%s, thread_num:%d", config_file_.data(), thread_num);
    return -40;
  }
  hlogd("config load thread_num, thread_num:%d", thread_num);
  server_thread_num_ = thread_num;

  // set addr
  auto addr = ini.GetValue("http_server_addr");
  if (addr.empty()) {
    hlogf("config load addr empty, file:%s", config_file_.data());
    return -40;
  }
  hlogd("config load addr, addr:%s", addr.data());
  server_addr_ = addr;

  // set js directory
  auto js_dir = ini.GetValue("js_directory");
  if (js_dir.empty()) {
    hlogf("config load js_directory empty, file:%s", config_file_.data());
    return -40;
  }
  hlogd("config load js_directory, js_directory:%s", js_dir.data());
  directory_ = js_dir;

  return 0;
}

}  // namespace core
}  // namespace xhworker
