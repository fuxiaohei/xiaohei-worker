#include <bindings/v8serviceworker/event/fetch_event.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <hv/hlog.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/isolate.h>
#include <v8wrap/js_value.h>
#include <webapi/event_target.h>
#include <webapi/fetch/fetch_response.h>

namespace v8serviceworker {

static void fetch_event_js_respondWith(const v8::FunctionCallbackInfo<v8::Value> &args) {
  // make event canceled after respondWith once
  auto event = v8wrap::get_ptr<webapi::Event>(args.Holder());
  event->cancel();

  if (v8wrap::valid_arglen(args, 1, "respondWith: ")) {
    return;
  }

  auto isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  auto reqScope = v8rt::getRequestScope(context);
  if (reqScope == nullptr) {
    v8wrap::throw_type_error(isolate, "respondWith: request scope is null");
    return;
  }

  if (args[0]->IsPromise()) {
    isolate->PerformMicrotaskCheckpoint();

    // TODO: handle promise
    return;
  }

  if (!v8wrap::IsolateData::IsInstanceOf(context, args[0], CLASS_FETCH_RESPONSE)) {
    v8wrap::throw_type_error(isolate, "respondWith: argument must be a Response");
    return;
  }
  auto response = v8wrap::get_ptr<webapi::FetchResponse>(args[0].As<v8::Object>());
  reqScope->set_response(response);
}

static void fetch_event_js_waitUntil(const v8::FunctionCallbackInfo<v8::Value> &args) {}

static void fetch_event_js_request_getter(const v8::FunctionCallbackInfo<v8::Value> &args) {}

void register_fetch_event(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuider) {
  auto isolate = isolateData->get_isolate();
  v8wrap::ClassBuilder fetchEventBuilder(isolate, CLASS_FETCH_EVENT);
  fetchEventBuilder.setConstructor(nullptr);
  fetchEventBuilder.setMethod("respondWith", fetch_event_js_respondWith);
  fetchEventBuilder.setMethod("waitUntil", fetch_event_js_waitUntil);
  fetchEventBuilder.setAccessorProperty("request", fetch_event_js_request_getter, nullptr, nullptr);
  auto fetch_event_template = fetchEventBuilder.getClassTemplate();

  isolateData->setClassTemplate(CLASS_FETCH_EVENT, fetch_event_template);
  classBuider->setMethod(CLASS_FETCH_EVENT, fetch_event_template);
}

}  // namespace v8serviceworker