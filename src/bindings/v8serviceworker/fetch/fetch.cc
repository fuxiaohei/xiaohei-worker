#include <bindings/v8serviceworker/fetch/fetch.h>
#include <bindings/v8serviceworker/fetch/fetch_response.h>
#include <bindings/v8serviceworker/serviceworker.h>
#include <runtime/v8rt/v8rt.h>
#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>

namespace v8serviceworker {

void register_fetch_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  auto responseTemplate = create_response_template(isolateData);
  classBuilder->setMethod(CLASS_FETCH_RESPONSE, responseTemplate);
}

webapi::FetchHeaders *init_headers(v8::Isolate *isolate, v8::Local<v8::Value> value) {
  auto *headers = v8rt::allocObject<webapi::FetchHeaders>(isolate);
  std::vector<std::pair<std::string, std::string>> header_values;

  if (v8wrap::convert_object_map(isolate->GetCurrentContext(), value, &header_values)) {
    for (auto &item : header_values) {
      headers->append(item.first, item.second);
    }
    return headers;
  }

  v8wrap::throw_type_error(isolate, "invalid sequence");
  return headers;
}

}  // namespace v8serviceworker