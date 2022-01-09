/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the MIT License. See License file in the project root for
 * license information.
 */

#pragma once

#include <hv/hlog.h>
#include <stdio.h>
#include <cstddef>
#include <type_traits>

namespace common {

class HeapObject {
 public:
    HeapObject() = default;
    virtual ~HeapObject() {}
};

struct HeapNode {
    HeapObject *data;
    HeapNode *next = nullptr;

    ~HeapNode() {
        delete data;
        if (next) {
            delete next;
        }
    }
};

class Heap {
 public:
    static Heap *Create() { return new Heap(); }

 public:
    template <class HeapObjectT> HeapObjectT *alloc() {
        static_assert(std::is_base_of<HeapObject, HeapObjectT>::value,
                      "Allocating Object must be inherited from heap::Object");

        auto sz = sizeof(HeapObjectT);
        m_alloc_size += sz;
        m_alloc_size += sizeof(HeapNode);

        hlogd("heap:%p, allocate %d bytes", this, m_alloc_size);

        if (m_begin == nullptr) {
            m_begin = new HeapNode();
            m_begin->data = new HeapObjectT();
            m_begin->next = nullptr;
            m_current = m_begin;
            return static_cast<HeapObjectT *>(m_current->data);
        }
        auto next = new HeapNode();
        next->data = new HeapObjectT();
        next->next = nullptr;
        m_current->next = next;
        m_current = next;
        return static_cast<HeapObjectT *>(next->data);
    }

    void free() {
        hlogd("heap:%p, free %d bytes", this, m_alloc_size);
        delete this;
    }

    size_t get_alloc_size() const { return m_alloc_size; }

 private:
    Heap() = default;
    ~Heap() {
        if (m_begin) {
            delete m_begin;
        }
    }

 private:
    HeapNode *m_begin = nullptr;
    HeapNode *m_current = nullptr;
    size_t m_alloc_size = 0;
};

}  // namespace common
