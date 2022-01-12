/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once
#include <map>
#include <string>

namespace core {
class Worker;

class Container {
 public:
  static Worker *GetWorker(const std::string &key);

 private:
  static Container *FromThreadLocal_();

  Worker *find_worker(const std::string &key);
  Worker *install_worker(const std::string &key);
  void terminate_worker(const std::string &key);
  std::map<std::string, Worker *> workers_;
};

}  // namespace core
