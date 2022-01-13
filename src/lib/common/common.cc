/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <common/common.h>

namespace common {

const char *error_message(int err) {
  switch (err) {
#define F(errcode, name, errmsg) \
  case errcode:                  \
    return errmsg;
    FOREACH_ERROR_STATUS(F)
#undef F
    default:
      return "undefined error";
  }
}

int create_id() {
  static std::atomic<int> id{1};
  return id.fetch_add(1, std::memory_order_relaxed);
}

}  // namespace common
