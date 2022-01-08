/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#pragma once

#include <v8.h>
#include <string>

#define V8WRAP_ISOLATE_MEMORY_MIN (1024 * 1024 * 8)
#define V8WRAP_ISOLATE_MEMORY_DEFAULT (1024 * 1024 * 128)

namespace v8wrap {

static const char *get_version() { return v8::V8::GetVersion(); }

// --- initialize

void init(const std::string &icu_file = "");
void shutdown();

// --- allocator

class Allocator : public v8::ArrayBuffer::Allocator {
 public:
    explicit Allocator(size_t maxSize = 0);
    ~Allocator();

    void *Allocate(size_t length) override;
    void *AllocateUninitialized(size_t length) override;
    void Free(void *data, size_t length) override;

    size_t get_current_size() const { return m_current_alloc; }

 private:
    size_t m_current_alloc = 0;
    size_t m_total_alloc = 0;
    size_t m_max_size = 0;

    v8::ArrayBuffer::Allocator *m_allocator = nullptr;
};

// --- isolate

v8::Isolate *new_isolate(v8::ArrayBuffer::Allocator *allocator,
                         size_t memory_size = V8WRAP_ISOLATE_MEMORY_DEFAULT);

// --- context

v8::Local<v8::Context> new_context(v8::Isolate *isolate);
v8::Local<v8::Context>
new_context(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global_template);

// -- run script

struct Result {
    v8::Local<v8::Value> value;
    bool is_exception = false;
    std::string exception_message;
    std::string exception_stack;
};

Result run_script(v8::Isolate *isolate, const std::string &script);
Result run_script(v8::Local<v8::Context> context, const std::string &script);

};  // namespace v8wrap
