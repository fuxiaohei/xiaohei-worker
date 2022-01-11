/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <common/common.h>
#include <core/request_scope.h>
#include <hv/hlog.h>
#include <runtime/v8rt/v8js_context.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_value.h>
#include <webapi/global_scope.h>

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

RuntimeContext *V8Runtime::get_context() {
  RuntimeContext *jsContext;

  // if queue is empty create new one
  if (context_queue_.empty()) {
    jsContext = V8JsContext::Create(this, content_, origin_);
    hlogd("v8js: create jsContext, iso:%p, jsCtx:%p, queue:%ld", isolate_, jsContext,
          context_queue_.size());
  } else {
    jsContext = context_queue_.front();
    context_queue_.pop();
    hlogd("v8js: get jsContext, iso:%p, jsCtx:%p, queue:%ld", isolate_, jsContext,
          context_queue_.size());
  }

  return jsContext;
}

void V8Runtime::recycle_context(RuntimeContext *context) {
  V8JsContext *jsContext = static_cast<V8JsContext *>(context);
  if (jsContext->get_error_code() != 0) {
    hlogd("v8js: recycle error jsContext, jsCtx:%p, error_code:%d, msg:%s", jsContext,
          jsContext->get_error_code(), jsContext->get_exception_msg().c_str());
    delete jsContext;
    return;
  }
  // push bach to queue
  context_queue_.push(jsContext);
  hlogd("v8js: recycle jsContext, iso:%p, jsCtx:%p, queue:%ld", isolate_, jsContext,
        context_queue_.size());
}

int V8Runtime::compile(const std::string &content, const std::string &origin) {
  content_ = content;
  origin_ = origin;

  auto jsContext = V8JsContext::Create(this, content_, origin_);
  if (jsContext == nullptr) {
    hlogw("v8js: compile nullptr, iso:%p, jsCtx:%p, error_code:%d, msg:%s", isolate_, jsContext,
          jsContext->get_error_code(), jsContext->get_exception_msg().c_str());
    return common::ERROR_RUNTIME_COMPILE_ERROR;
  }

  if (jsContext->get_error_code() != 0) {
    int error_code = jsContext->get_error_code();
    compile_error_message_ = jsContext->get_exception_msg();
    hlogw("v8js: compile error, error_code:%d, msg:%s", error_code, compile_error_message_.c_str());
    delete jsContext;
    return error_code;
  }

  context_queue_.push(jsContext);
  hlogd("v8js: create jsContext, iso:%p, jsCtx:%p, queue:%ld, result:%s", isolate_, jsContext,
        context_queue_.size(), jsContext->get_compiled_result().data());
  return 0;
}

// --- single methods

common::Heap *getHeap(v8::Local<v8::Context> context) {
  // allocate request scope
  auto req_scope =
      v8wrap::get_ptr<xhworker::core::RequestScope>(context, V8_JS_CONTEXT_REQUEST_SCOPE_INDEX);
  if (req_scope != nullptr) {
    return req_scope->heap_;
  }

  // alloc by global scope
  auto scope =
      v8wrap::get_ptr<webapi::ServiceWorkerGlobalScope>(context, V8_JS_CONTEXT_JSGLOBAL_INDEX);
  if (scope == nullptr) {
    return nullptr;
  }
  return scope->heap_;
}

}  // namespace runtime
}  // namespace xhworker