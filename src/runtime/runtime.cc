/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <runtime/conf.h>
#include <runtime/runtime.h>

#if BUILD_V8_RUNTIME
#include <runtime/v8rt/v8rt.h>
#endif

namespace xhworker {

namespace runtime {

Runtime *Runtime::create(int rttype, const Options &options) {
#if BUILD_V8_RUNTIME
  if (rttype == RT_JS_V8) {
    return new V8Runtime(options);
  }
#endif

  return nullptr;
}

void Runtime::release(Runtime *runtime) {}

}  // namespace runtime
}  // namespace xhworker

