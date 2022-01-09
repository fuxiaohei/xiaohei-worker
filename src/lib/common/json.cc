/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#include <common/json.h>
#include <hv/hfile.h>

namespace common {

int read_file(const std::string &filepath, std::string *content) {
    HFile file;
    if (file.open(filepath.data(), "r") != 0) {
        return -1;
    }
    file.readall(*content);
    return 0;
}

int json_parse(const std::string &str, nlohmann::json *json, std::string *errmsg) {
    try {
        *json = nlohmann::json::parse(str);
    } catch (nlohmann::detail::exception &e) {
        *errmsg = e.what();
        return -1;
    }
    return (json->is_discarded() || json->is_null()) ? -2 : 0;
}

int json_parse_file(const std::string &fpath, nlohmann::json *json, std::string *errmsg) {
    std::string file_content;
    auto flag = read_file(fpath, &file_content);
    if (flag != 0) {
        return flag;
    }
    return json_parse(file_content, json, errmsg);
}

std::string json_encode(const nlohmann::json &json, int indent) { return json.dump(indent); }

}  // namespace common
