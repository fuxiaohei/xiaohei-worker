/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <v8wrap/js_class.h>
#include <v8wrap/js_value.h>

namespace v8wrap {

static void empty_ctor(const v8::FunctionCallbackInfo<v8::Value> &args) {}

ClassBuilder::ClassBuilder(v8::Isolate *isolate, const std::string &name)
    : class_name_(name), isolate_(isolate) {
    assert(name.size() > 0 && "js class requries name");
}

void ClassBuilder::setConstructor(v8::FunctionCallback callback, void *data) {
    if (callback == nullptr) {
        callback = empty_ctor;
    }
    class_template_ = new_function_template(isolate_, callback, data);

    // set internal field count default
    class_template_->InstanceTemplate()->SetInternalFieldCount(2);  // 2 is enough now

    // set class name
    auto classname = new_string(isolate_, class_name_);
    class_template_->SetClassName(classname);
    class_template_->PrototypeTemplate()->Set(
        v8::Symbol::GetToStringTag(isolate_), classname,
        static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontEnum));
}

void ClassBuilder::setValue(const std::string &name, v8::Local<v8::Value> value) {
    assert(!class_template_.IsEmpty() && "js class constructor is not set");
    assert(name.size() > 0 && "js class value requries property name");
    class_template_->PrototypeTemplate()->Set(isolate_, name.data(), value);
}

void ClassBuilder::setAccessorProperty(const std::string &name, v8::FunctionCallback getter,
                                       v8::FunctionCallback setter, void *data) {
    assert(!class_template_.IsEmpty() && "js class constructor is not set");
    assert(name.size() > 0 && "js class accessor property requries property name");
    assert(getter != nullptr || setter != nullptr);
    class_template_->PrototypeTemplate()->SetAccessorProperty(
        new_string(isolate_, name),
        new_function_template(isolate_, getter, data),
        new_function_template(isolate_, setter, data));
}

void ClassBuilder::setMethod(const std::string &name, v8::FunctionCallback callback, void *data) {
    assert(!class_template_.IsEmpty() && "js class constructor is not set");
    assert(name.size() > 0 && "js class method requries property name");
    assert(callback != nullptr && "js class method requries callback");
    class_template_->PrototypeTemplate()->Set(isolate_, name.data(),
                                              new_function_template(isolate_, callback, data));
}

void ClassBuilder::setMethod(const std::string &name, v8::Local<v8::FunctionTemplate> fn_template) {
    assert(!class_template_.IsEmpty() && "js class constructor is not set");
    assert(name.size() > 0 && "js class method requries property name");
    class_template_->PrototypeTemplate()->Set(isolate_, name.data(), fn_template);
}

void ClassBuilder::setStaticMethod(const std::string &name, v8::FunctionCallback callback,
                                   void *data) {
    assert(!class_template_.IsEmpty() && "js class constructor is not set");
    assert(name.size() > 0 && "js class method requries property name");
    assert(callback != nullptr && "js class method requries callback");
    class_template_->Set(isolate_, name.data(), new_function_template(isolate_, callback, data));
}

void ClassBuilder::setStaticMethod(const std::string &name,
                                   v8::Local<v8::FunctionTemplate> fn_template) {
    assert(!class_template_.IsEmpty() && "js class constructor is not set");
    assert(name.size() > 0 && "js class method requries property name");
    class_template_->Set(isolate_, name.data(), fn_template);
}

void ClassBuilder::inherit(v8::Local<v8::FunctionTemplate> parent) {
    assert(!class_template_.IsEmpty() && "js class constructor is not set");
    class_template_->Inherit(parent);
}

}  // namespace v8wrap
