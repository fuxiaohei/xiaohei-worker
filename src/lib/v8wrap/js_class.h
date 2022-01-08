/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>
#include <string>

namespace v8wrap {

class ClassBuilder {
 public:
    ClassBuilder(v8::Isolate *isolate, const std::string &name);
    ~ClassBuilder() = default;

    void setConstructor(v8::FunctionCallback callback, void *data = nullptr);
    void setValue(const std::string &name, v8::Local<v8::Value> value);
    void setAccessorProperty(const std::string &name, v8::FunctionCallback getter,
                             v8::FunctionCallback setter, void *data = nullptr);
    void setMethod(const std::string &name, v8::FunctionCallback callback, void *data = nullptr);
    void setMethod(const std::string &name, v8::Local<v8::FunctionTemplate> fn_template);
    void setStaticMethod(const std::string &name, v8::FunctionCallback callback,
                         void *data = nullptr);
    void setStaticMethod(const std::string &name, v8::Local<v8::FunctionTemplate> fn_template);
    void inherit(v8::Local<v8::FunctionTemplate> parent);

    v8::Local<v8::FunctionTemplate> getClassTemplate() {
        assert(!class_template_.IsEmpty() && "js class constructor is not set");
        return class_template_;
    }

 private:
    std::string class_name_;
    v8::Isolate *isolate_ = nullptr;
    v8::Local<v8::FunctionTemplate> class_template_;
};

};  // namespace v8wrap
