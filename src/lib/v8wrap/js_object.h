/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>
#include <string>
#include <utility>
#include <vector>

namespace v8wrap {

bool has_property(v8::Local<v8::Object> object, const std::string &name);
v8::Local<v8::Value> get_property(v8::Local<v8::Object> object, const std::string &name);
void set_property(v8::Local<v8::Object> object, const std::string &name,
                  v8::Local<v8::Value> value);

bool convert_object_map(v8::Local<v8::Context> context, v8::Local<v8::Value> value,
                        std::vector<std::pair<std::string, std::string>> *map);

}  // namespace v8wrap
