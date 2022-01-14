/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <common/common.h>
#include <runtime/fetch_backend.h>
#include <runtime/v8rt/v8js_fetch.h>
#include <runtime/v8rt/v8rt.h>
#include <v8.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_value.h>
#include <webapi/fetch/fetch_response.h>

namespace v8rt {

V8FetchContext::V8FetchContext(core::RequestScope *req_scope,
                               v8::Local<v8::Promise::Resolver> resolver)
    : reqScope_(req_scope) {
  retain();

  fetchReqID_ = common::create_id();
  runtime::FetchBackend::Save(fetchReqID_, this);
  reqScope_->append_fetch_request(fetchReqID_);

  isolate_ = resolver->GetIsolate();
  context_.Set(isolate_, isolate_->GetCurrentContext());
  promise_resolver_.Set(isolate_, resolver);

  // save current event loop
  // it need callback and run isoalte in this event loop,
  // but http request is async in another thread
  mainloop_ = hv::currentThreadEventLoop;

  req_.reset(new HttpRequest());
  req_->method = HTTP_GET;
  req_->timeout = 10;

  hlogd("v8js_fetch: created, id: %d, refCount:%d", fetchReqID_, get_ref_count());
}

void V8FetchContext::set_url(const std::string &url) { req_->url = url; }

void V8FetchContext::do_request() {
  if (is_sent_.load()) {
    hlogd("v8js_fetch: send_async_once: already send, id: %d", fetchReqID_);
    return;
  }
  is_sent_.store(true);

  retain();

  // set keepalive
  if (req_->GetHeader("Connection").empty()) {
    req_->SetHeader("Connection", "keep-alive");
  }

  async_client_.sendAsync(req_, [this](const HttpResponsePtr &resp) {
    if (resp == nullptr) {
      // FIXME:: set error ?
      return;
    }
    resp_ = resp;

    hlogd("v8js_fetch: send_async_once: response, id: %d, resp: %p, refCount:%d", fetchReqID_,
          resp.get(), get_ref_count());

    // make sure isolate is used in same thread,
    // it need notify http request thread to handle fetch response to fulfill promise.
    notify_request_done();
  });
}

void V8FetchContext::terminate() { release(); }

void V8FetchContext::destroy() {
  hlogd("v8js_fetch: destroy, id: %d, refCount:%d", fetchReqID_, get_ref_count());
  delete this;
}

void V8FetchContext::notify_request_done() {
  if (mainloop_ == nullptr) {
    release();
    return;
  }

  hlogd("v8js_fetch: notify_request_done, id: %d", fetchReqID_);
  mainloop_->queueInLoop(std::bind(&V8FetchContext::fulfill_promise, this));
}

void V8FetchContext::fulfill_promise() {
  v8::HandleScope handle_scope(isolate_);
  v8::TryCatch try_catch(isolate_);

  auto context = context_.Get(isolate_);
  auto resolver = promise_resolver_.Get(isolate_);

  // create response object
  v8::Local<v8::Value> response_obj;

  if (!v8wrap::IsolateData::NewInstance(context, CLASS_FETCH_RESPONSE, &response_obj)) {
    resolver->Reject(context, v8wrap::new_type_error(isolate_, "failed to create Response object"))
        .Check();
    hlogw("v8js_fetch: failed to create Response object, id: %d", fetchReqID_);
  } else {
    // set response
    auto response = allocObject<webapi::FetchResponse>(context);
    response->setBody(resp_->Body());
    v8wrap::set_ptr(response_obj.As<v8::Object>(), response);
    // fullfil response
    resolver->Resolve(context, response_obj).Check();
    hlogi("v8js_fetch: fulfill_promise: done, id: %d, resp: %p, refCount:%d", fetchReqID_,
          resp_.get(), get_ref_count());
  }

  // request scope all waitings
  reqScope_->handle_waitings();

  release();
}

}  // namespace v8rt