/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <v8wrap/js_object.h>
#include <v8wrap/js_value.h>

namespace v8wrap {

bool has_property(v8::Local<v8::Object> object, const std::string &name) {
    return object
        ->Has(object->GetIsolate()->GetCurrentContext(), new_string(object->GetIsolate(), name))
        .FromMaybe(false);
}

v8::Local<v8::Value> get_property(v8::Local<v8::Object> object, const std::string &name) {
    return object
        ->Get(object->GetIsolate()->GetCurrentContext(), new_string(object->GetIsolate(), name))
        .ToLocalChecked();
}

void set_property(v8::Local<v8::Object> object, const std::string &name,
                  v8::Local<v8::Value> value) {
    object
        ->Set(object->GetIsolate()->GetCurrentContext(), new_string(object->GetIsolate(), name),
              value)
        .ToChecked();
}

bool convert_object_map(v8::Local<v8::Context> context, v8::Local<v8::Value> value,
                        std::vector<std::pair<std::string, std::string>> *map) {
    if (!is_sequence(context, value)) {
        return false;
    }
    if (value->IsArray()) {
        auto arr = value.As<v8::Array>();
        for (uint32_t i = 0; i < arr->Length(); i++) {
            auto item = arr->Get(context, i).ToLocalChecked();
            if (!item->IsArray()) {
                return false;
            }
            auto arr = item.As<v8::Array>();
            if (arr->Length() != 2) {
                return false;
            }
            std::string key = v8wrap::to_string(context, arr->Get(context, 0).ToLocalChecked());
            std::string value = v8wrap::to_string(context, arr->Get(context, 1).ToLocalChecked());
            map->push_back(std::make_pair(key, value));
        }
        return true;
    }
    if (value->IsObject()) {
        auto obj = value.As<v8::Object>();
        auto keys = obj->GetPropertyNames(context).ToLocalChecked();
        for (uint32_t i = 0; i < keys->Length(); i++) {
            auto key_value = keys->Get(context, i).ToLocalChecked();
            std::string key = v8wrap::to_string(context, key_value);

            auto elem_value = obj->Get(context, key_value).ToLocalChecked();
            std::string value = v8wrap::to_string(context, elem_value);

            map->push_back(std::make_pair(key, value));
        }
        return true;
    }
    return false;
}

}  // namespace v8wrap
