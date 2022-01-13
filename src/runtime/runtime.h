/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <core/request_scope.h>

#include <string>

namespace webapi {
class FetchResponse;
}

namespace runtime {

enum {
  RT_JS_V8 = 1,
  RT_WASM_V8 = 2,
};

struct Options {
  size_t memory_limit = 0;
  int cpu_time = 0;
};

class FetchContext {
 public:
  FetchContext() = default;
  virtual ~FetchContext() {}

 public:
  virtual int get_id() = 0;
  virtual void set_url(const std::string &url) = 0;
  virtual void do_request() = 0;
  virtual bool is_do_requested() = 0;
  virtual void terminate() = 0;
};

class RuntimeContext {
 public:
  RuntimeContext() = default;
  virtual ~RuntimeContext() {}

 public:
  virtual int handle_http_request(core::RequestScope *reqscope) = 0;
  virtual int get_error_code() const = 0;
  virtual void recycle() = 0;
  virtual webapi::FetchResponse *get_promised_respose() = 0;
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
