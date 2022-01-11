/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <v8wrap/js_value.h>

namespace v8wrap {

v8::Local<v8::String> new_string(v8::Isolate *isolate, const char *chars, size_t length) {
    return v8::String::NewFromUtf8(isolate, chars, v8::NewStringType::kNormal, length)
        .ToLocalChecked();
}

v8::Local<v8::String> new_string(v8::Isolate *isolate, const std::string &str) {
    return new_string(isolate, str.data(), str.size());
}

std::string get_string(v8::Isolate *isolate, v8::Local<v8::Value> value) {
    if (!value->IsString()) {
        return std::string();
    }

    std::string out;
    v8::Local<v8::String> str = v8::Local<v8::String>::Cast(value);
    int length = str->Utf8Length(isolate);
    out.resize(length);
    str->WriteUtf8(isolate, &(out)[0], length, nullptr, v8::String::NO_NULL_TERMINATION);
    return out;
}

std::string to_string(v8::Local<v8::Context> context, v8::Local<v8::Value> value) {
    auto string_value = value->ToString(context).ToLocalChecked();
    return get_string(context->GetIsolate(), string_value);
}

v8::Local<v8::Integer> new_int32(v8::Isolate *isolate, int32_t value) {
    return v8::Integer::New(isolate, value);
}

int32_t get_int32(v8::Local<v8::Value> value) {
    if (value->IsInt32()) {
        return value.As<v8::Int32>()->Value();
    }
    if (!value->IsNumber()) {
        return 0;
    }
    v8::Local<v8::Context> context;
    return value->Int32Value(context).FromJust();
}

void get_exception(v8::Local<v8::Context> context, v8::TryCatch *try_catch, std::string *message,
                   std::string *stack) {
    if (try_catch->Exception().IsEmpty()) {
        return;
    }
    *message = to_string(context, try_catch->Exception());
    *stack = to_string(context, try_catch->StackTrace(context).ToLocalChecked());
}

v8::Local<v8::Value> new_type_error(v8::Isolate *isolate, const std::string &message) {
    return v8::Exception::TypeError(v8wrap::new_string(isolate, message));
}

void throw_type_error(v8::Isolate *isolate, const std::string &message) {
    isolate->ThrowException(new_type_error(isolate, message));
}

bool throw_valid_length(const v8::FunctionCallbackInfo<v8::Value> &args, int expect,
                        const std::string &desc) {
    if (args.Length() >= expect) {
        return false;
    }
    std::string message = std::string(desc);
    message.append(std::to_string(expect));
    message.append(" argument required, but only ");
    message.append(std::to_string(args.Length()));
    message.append(" present.");
    args.GetIsolate()->ThrowException(v8::Exception::Error(new_string(args.GetIsolate(), message)));
    return true;
}

v8::Local<v8::FunctionTemplate> new_function_template(v8::Isolate *isolate, v8::FunctionCallback fn,
                                                      void *data) {
    if (data == nullptr) {
        return v8::FunctionTemplate::New(isolate, fn);
    }
    return v8::FunctionTemplate::New(isolate, fn, v8::External::New(isolate, data));
}

bool is_sequence(v8::Local<v8::Context> context, v8::Local<v8::Value> value) {
    if (value->IsArray()) {
        auto arr = value.As<v8::Array>();
        if (arr->Length() > 0) {
            return true;
        }
        auto first = arr->Get(context, 0).ToLocalChecked();
        return (first->IsArray() && first.As<v8::Array>()->Length() == 2);
    }
    return value->IsObject();
}

}  // namespace  v8wrap
