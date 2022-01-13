/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <common/common.h>
#include <core/request_scope.h>
#include <hv/hlog.h>

namespace core {

RequestScope::RequestScope(const HttpContextPtr& ctx) : ctx_(ctx) {
  heap_ = common::Heap::Create();
}

void RequestScope::destroy() {
  hlogw("RequestScope::destroy %p", this);
  delete this;
}

RequestScope::~RequestScope() {
  heap_->free();
  heap_ = nullptr;
}

webapi::FetchEvent* RequestScope::create_fetch_event() {
  auto fetchEvent = heap_->alloc<webapi::FetchEvent>();
  fetchEvent->data = this;
  return fetchEvent;
}

void RequestScope::set_error_msg(int errcode, const std::string& msg, const std::string& stack) {
  error_code_ = errcode;
  error_msg_ = msg;
  error_stack = stack;
  hlogw("RequestScope::set_error_msg %p, code:%d, msg:%s, stack:%s", this, errcode, msg.c_str(),
        stack.c_str());
}

std::string RequestScope::error_response_body() {
  std::string message = common::error_message(error_code_);
  if (!error_stack.empty()) {
    message += "\n" + error_stack;
    return message;
  }
  if (!error_msg_.empty()) {
    message += "\n" + error_msg_;
  }
  return message;
}

int RequestScope::handle_response() {
  if (is_response_sent_.load()) {
    return response_status_code_;
  }

  if (error_code_ != 0) {
    is_response_sent_.store(true);
    ctx_->setBody(error_response_body());
    response_status_code_ = HTTP_STATUS_INTERNAL_SERVER_ERROR;
    return response_status_code_;
  }

  if (response_ != nullptr) {
    is_response_sent_.store(true);
    response_status_code_ = response_->getRawResponse()->status_code;
    ctx_->writer->Begin();
    ctx_->writer->WriteResponse(response_->getRawResponse());
    ctx_->writer->End();
    return response_status_code_;
  }
  
  return 0;
}

}  // namespace core
