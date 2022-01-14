/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/base64/base64.h>
#include <hv/base64.h>
#include <v8wrap/js_value.h>

#include <string>

namespace v8serviceworker {

void js_atob(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto isolate = args.GetIsolate();
  if (v8wrap::valid_arglen(args, 1, "atob: ")) {
    return;
  }
  std::string encoded = v8wrap::to_string(isolate->GetCurrentContext(), args[0]);
  std::string decoded = hv::Base64Decode(encoded.c_str(), encoded.size());
  if (!decoded.empty()) {
    args.GetReturnValue().Set(v8wrap::new_string(isolate, decoded));
    return;
  }
  v8wrap::throw_type_error(isolate, "btoa: invalid input");
}
void js_btoa(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto *isolate = args.GetIsolate();
  if (v8wrap::valid_arglen(args, 1, "btoa: ")) {
    return;
  }
  std::string raw = v8wrap::to_string(isolate->GetCurrentContext(), args[0]);
  std::string encoded = hv::Base64Encode((const unsigned char *)raw.data(), raw.size());

  args.GetReturnValue().Set(v8wrap::new_string(isolate, encoded));
}

void register_base64_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  classBuilder->setMethod("atob", js_atob);
  classBuilder->setMethod("btoa", js_btoa);
}

};  // namespace v8serviceworker
