/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#pragma once

#include <hv/json.hpp>
#include <string>

namespace common {

int read_file(const std::string &filepath, std::string *content);

int json_parse(const std::string &str, nlohmann::json *json, std::string *errmsg);
int json_parse_file(const std::string &fpath, nlohmann::json *json, std::string *errmsg);
std::string json_encode(const nlohmann::json &json, int indent);

}  // namespace common
