/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <string>

namespace xhworker {
namespace runtime {

enum {
  RT_JS_V8 = 1,
  RT_WASM_V8 = 2,
};

struct Options {
  size_t memory_limit = 0;
  int cpu_time = 0;
};

class RuntimeContext {
 public:
  RuntimeContext() = default;
  virtual ~RuntimeContext() {}
};

class Runtime {
 public:
  static Runtime *create(int rttype, const Options &options);
  static void release(Runtime *runtime);

 public:
  Runtime() = default;
  virtual ~Runtime() {}

 public:
  virtual RuntimeContext *get_context() = 0;
  virtual void recycle_context(RuntimeContext *context) = 0;
  virtual int compile(const std::string &content, const std::string &origin) = 0;
  virtual std::string get_compile_error_message() = 0;
};

}  // namespace runtime
}  // namespace xkworker