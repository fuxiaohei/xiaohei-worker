/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#pragma once

#include <atomic>

namespace common {

#define FOREACH_ERROR_STATUS(F)                          \
  F(0, OK, "ok")                                         \
  F(-1, MANIFEST_NOT_FOUND, "worker is not found")       \
  F(-3, MANIFEST_ERROR, "worker manifest parse failed")  \
  F(-5, ENTRY_NOT_FOUND, "worker entry is not found")    \
  F(-7, ENTRY_READ_ERROR, "worker entry read failed")    \
  F(-11, RUNTIME_UNKNOWN, "runtime unknown")             \
  F(-13, RUNTIME_COMPILE_ERROR, "runtime compile error") \
  F(-21, HTTP_INVALID_RESPONSE, "invalid response")      \
  F(-31, V8JS_THREW_EXCEPTION, "v8: js threw exception")

enum {
#define F(errcode, name, errmsg) ERROR_##name = errcode,
  FOREACH_ERROR_STATUS(F)
#undef F
};

const char *error_message(int err);

class RefCounted {
 public:
  void retain() { ref_count_++; }
  void release() {
    if (--ref_count_ == 0) {
      destroy();
    }
  }

 protected:
  virtual void destroy() = 0;
  int get_ref_count() { return ref_count_.load(); }

  std::atomic<int> ref_count_ = {0};
};

}  // namespace common
