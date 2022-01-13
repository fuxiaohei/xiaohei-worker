/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <runtime/runtime.h>

#include <string>

namespace runtime {

class FetchBackend {
 public:
  static void Save(int id, FetchContext *context);
  static int Call(int id);
  static int Termiate(int id);

 private:
  static FetchBackend *getInstance();

 private:
  explicit FetchBackend() = default;
  ~FetchBackend() = default;

 private:
  void save(int id, FetchContext *context);
  int call(int id);
  int terminate(int id);

 private:
  std::map<int, FetchContext *> contexts_;
};

}  // namespace runtime