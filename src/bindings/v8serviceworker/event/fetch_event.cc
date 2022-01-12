#include <bindings/v8serviceworker/event/fetch_event.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <hv/hlog.h>

namespace v8serviceworker {

static void fetch_event_js_respondWith(const v8::FunctionCallbackInfo<v8::Value> &args) {
  hlogw("fetch_event_js_respondWith");
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