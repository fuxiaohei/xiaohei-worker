/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <common/common.h>
#include <core/request_scope.h>
#include <hv/hlog.h>
#include <runtime/runtime.h>

namespace core {

RequestScope::RequestScope(const HttpContextPtr& ctx) : ctx_(ctx) {
  heap_ = common::Heap::Create();
}

int RequestScope::destroy() {
  hlogi("RequestScope::destroy %p", this);
  delete this;
  return 0;
}

RequestScope::~RequestScope() {
  if (runtime_context_) {
    runtime_context_->recycle();
  }

  terminate_fetch_requests();

  heap_->free();
  heap_ = nullptr;
}

webapi::FetchEvent* RequestScope::create_fetch_event() {
  auto fetchEvent = heap_->alloc<webapi::FetchEvent>();
  fetchEvent->data = this;
  return fetchEvent;
}

webapi::FetchRequest* RequestScope::create_fetch_request() {
  auto fetchRequest = heap_->alloc<webapi::FetchRequest>();
  fetchRequest->set_request(ctx_->request.get());
  return fetchRequest;
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
    hlogw("RequestScope::handle_response %p, response already sent", this);
    return response_status_code_;
  }

  if (error_code_ != 0) {
    is_response_sent_.store(true);
    hlogw("RequestScope::handle_response %p, error:%d, msg:%s", this, error_code_,
          error_msg_.c_str());

    ctx_->setBody(error_response_body());
    response_status_code_ = HTTP_STATUS_INTERNAL_SERVER_ERROR;
    return response_status_code_;
  }

  if (response_ != nullptr) {
    is_response_sent_.store(true);
    hlogd("RequestScope::handle_response %p, response:%p", this, response_);

    response_status_code_ = response_->getRawResponse()->status_code;
    ctx_->writer->Begin();
    ctx_->writer->WriteResponse(response_->getRawResponse());
    ctx_->writer->End();
    return response_status_code_;
  }

  return 0;
}

void RequestScope::handle_waitings() {
  do_fetch_requests();

  // if response is not sent, need get response from promise maybe
  if (!is_response_sent_.load()) {
    hlogd("RequestScope::handle_waitings %p, response is not sent", this);
    if (runtime_context_) {
      auto response = runtime_context_->get_promised_respose();
      if (response != nullptr) {
        response_ = response;
        hlogd("RequestScope::handle_waitings %p, get response from promise", this);
        handle_response();
      }
    }
    if (!is_response_sent_.load() && error_code_ != 0) {
      hlogd("RequestScope::handle_waitings %p, error_code_ != 0, error_msg_:%s", this,
            error_msg_.c_str());
      handle_response();
    }
  } else {
    hlogd("RequestScope::handle_waitings %p, response is sent", this);
  }

  if (is_response_sent_.load()) {
    hlogd("RequestScope::handle_waitings %p, response is done", this);
    destroy();
  }
}

void RequestScope::save_fetch_request(runtime::FetchContext* fetchContext) {
  fetch_requests_.push_back(fetchContext);
}

void RequestScope::do_fetch_requests() {
  for (auto fetchCtx : fetch_requests_) {
    if (!fetchCtx->is_do_requested()) {
      fetchCtx->do_request();
    }
  }
}

void RequestScope::terminate_fetch_requests() {
  for (auto fetchCtx : fetch_requests_) {
    fetchCtx->terminate();
  }
}

}  // namespace core
