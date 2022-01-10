/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <hv/hlog.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>

namespace xhworker {
namespace runtime {

V8Runtime::V8Runtime(const Options &options) {  // create a new isolate
  size_t memory_size = V8_JS_ISOLATE_MEMORY_LIMIT;
  if (options.memory_limit > 0) {
    memory_size = options.memory_limit;  // in mb
  }

  allocator_ = new v8wrap::Allocator();
  isolate_ = v8wrap::new_isolate(allocator_, memory_size * 1024 * 1024);

  hlogd("v8js: create isolate, iso:%p, alloc:%p, memory_size:%zu", isolate_, allocator_,
        memory_size);
}

V8Runtime::~V8Runtime() {
  hlogd("v8js: dispose isolate, iso:%p, alloc:%p", isolate_, allocator_);
  v8wrap::IsolateData::Release(isolate_);
  isolate_->Dispose();
  isolate_ = nullptr;
  delete allocator_;
  allocator_ = nullptr;
}

RuntimeContext *V8Runtime::get_context() { return nullptr; }

void V8Runtime::recycle_context(RuntimeContext *context) {}

int V8Runtime::compile(const std::string &content, const std::string &origin) { return 0; }

}  // namespace runtime
}  // namespace xhworker