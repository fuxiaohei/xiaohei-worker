/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/timer.h>
#include <common/common.h>
#include <core/request_scope.h>
#include <hv/EventLoop.h>
#include <hv/hlog.h>
#include <runtime/runtime.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_value.h>

#include <string>

namespace v8serviceworker {

class JsTimer : public common::HeapObject, runtime::Timer {
 public:
  void create(v8::Isolate *isolate, v8::Local<v8::Function> callback);
  void start(core::RequestScope *reqScope, int timeout, bool is_interval);

  void execute();
  void terminate() { is_terminated = true; }

  hv::TimerID get_timer_id() const { return timer_id_; }

 private:
  friend class common::Heap;
  JsTimer() = default;
  ~JsTimer();

 private:
  hv::TimerID timer_id_ = 0;
  core::RequestScope *reqScope_ = nullptr;
  std::atomic<bool> is_terminated = {false};

  v8::Isolate *isolate_ = nullptr;
  v8::Eternal<v8::Function> callback_;
  v8::Eternal<v8::Context> context_;
};

JsTimer::~JsTimer() {
  if (timer_id_ != 0) {
    hlogd("timer: gc timer, scope:%p, tid:%lld", reqScope_, timer_id_);
    hv::killTimer(timer_id_);
  }
}

void JsTimer::create(v8::Isolate *isolate, v8::Local<v8::Function> callback) {
  isolate_ = isolate;
  context_.Set(isolate, isolate->GetCurrentContext());
  callback_.Set(isolate, callback);
}

void JsTimer::start(core::RequestScope *reqScope, int timeout, bool is_interval) {
  reqScope_ = reqScope;
  if (is_interval) {
    timer_id_ = hv::setInterval(timeout, [this](hv::TimerID) { execute(); });
    hlogd("timer: add interval timer, t:%d, scope:%p, tid:%lld", timeout, reqScope_, timer_id_);
    return;
  }
  timer_id_ = hv::setTimeout(timeout, [this](hv::TimerID) { execute(); });
  hlogd("timer: add timeout timer, t:%d, scope:%p, tid:%lld", timeout, reqScope_, timer_id_);
}

void JsTimer::execute() {
  if (is_terminated.load()) {
    hlogd("timer: timer is terminated, scope:%p, tid:%lld", reqScope_, timer_id_);
    return;
  }

  hlogd("timer: callback timer, scope:%p, tid:%lld", reqScope_, timer_id_);

  v8::HandleScope handle_scope(isolate_);
  v8::Local<v8::Context> context = context_.Get(isolate_);
  v8::Context::Scope context_scope(context);
  v8::Local<v8::Function> callback = callback_.Get(isolate_);
  v8::Local<v8::Value> argv[] = {};
  auto value = callback->Call(context, context->Global(), 0, argv);

  // after timercallback, isolate need run microtask
  isolate_->PerformMicrotaskCheckpoint();

  // if m_reqScope is waiting, it needs continue
  reqScope_->handle_waitings();
}

static void js_set_timer(const v8::FunctionCallbackInfo<v8::Value> &args, bool is_interval) {
  auto *isolate = args.GetIsolate();
  // create v8 js timer
  auto timeout = args[1].As<v8::Int32>()->Value();
  auto timer = v8rt::allocObject<JsTimer>(isolate);
  timer->create(isolate, args[0].As<v8::Function>());

  // allocate request scope
  auto reqScope = v8rt::getRequestScope(isolate->GetCurrentContext());

  timer->start(reqScope, timeout, is_interval);

  args.GetReturnValue().Set(v8wrap::new_int32(args.GetIsolate(), timer->get_timer_id()));
}

static void js_kill_timer(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t tid) {
  hv::killTimer(tid);
  hlogd("timer: kill timer, tid:%lld", tid);
}

static void js_setTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (v8wrap::valid_arglen(args, 2, "setTimeout: ")) {
    return;
  }
  auto *isolate = args.GetIsolate();
  if (!args[0]->IsFunction()) {
    v8wrap::throw_type_error(isolate, "setTimeout first argument must be a function");
    return;
  }
  if (!args[1]->IsNumber()) {
    v8wrap::throw_type_error(isolate, "setTimeout second argument must be a number");
    return;
  }
  js_set_timer(args, false);
}

static void js_clearTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (v8wrap::valid_arglen(args, 1, "clearTimeout: ")) {
    return;
  }
  if (!args[0]->IsNumber()) {
    v8wrap::throw_type_error(args.GetIsolate(), "clearTimeout: first argument must be a number");
    return;
  }
  auto tid = args[0].As<v8::Int32>()->Value();
  js_kill_timer(args, tid);
}

static void js_setInterval(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (v8wrap::valid_arglen(args, 2, "setInterval: ")) {
    return;
  }
  auto *isolate = args.GetIsolate();
  if (!args[0]->IsFunction()) {
    v8wrap::throw_type_error(isolate, "setInterval first argument must be a function");
    return;
  }
  if (!args[1]->IsNumber()) {
    v8wrap::throw_type_error(isolate, "setInterval second argument must be a number");
    return;
  }
  js_set_timer(args, true);
}

static void js_clearInterval(const v8::FunctionCallbackInfo<v8::Value> &args) {
  if (v8wrap::valid_arglen(args, 1, "clearInterval: ")) {
    return;
  }
  if (!args[0]->IsNumber()) {
    v8wrap::throw_type_error(args.GetIsolate(), "clearInterval: first argument must be a number");
    return;
  }
  auto tid = args[0].As<v8::Int32>()->Value();
  js_kill_timer(args, tid);
}

void register_timer_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  classBuilder->setMethod("setTimeout", js_setTimeout);
  classBuilder->setMethod("clearTimeout", js_clearTimeout);
  classBuilder->setMethod("setInterval", js_setInterval);
  classBuilder->setMethod("clearInterval", js_clearInterval);
}

};  // namespace v8serviceworker
