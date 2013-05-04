// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_TREE_H_
#define SRC_TREE_H_

#include <cstdint>
#include <cassert>

#include <iostream>
#include <mutex>

template <typename D>
struct Node {
    const Node<D> *parent;
    D data;
};


template <typename D>
class MemoryManager {
 public:
    MemoryManager();
    ~MemoryManager();
    void init(size_t capacity);

    Node<D> *alloc(const Node<D> *parent);
    void free(Node<D> *ptr);

    bool has_cycle(const Node<D> *start);

 private:
#pragma pack(push)
#pragma pack(1)
    struct Element {
        union {
            Element *next;
            size_t refs;
        } header;
        Node<D> data;
    };
#pragma pack(pop)

    static void inc_refs(Node<D> *node);
    static size_t dec_refs(Node<D> *node);

    int refs_;
    size_t capacity_;
    Element *area_;
    Element *free_list_;
    std::recursive_mutex mutex_;
};

#include "tree-inl.h"

#endif  // SRC_TREE_H_
