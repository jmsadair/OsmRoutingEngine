#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

// This struct is used to store basic information about an item in the Queue.
struct HeapElement {

    // This attribute is used during the bidirectional search. A 1 indicates forward search and a 0 indicates backward.
    int direction;

    // Used to identify an item in the Queue. Typically, this ID is the ID of an OSM node.
    uint64_t id;

    // Priority of an item in the Queue.
    double value;

    HeapElement(const uint64_t id, const double value) : id(id), value(value), direction(-1) {}

    HeapElement(const uint64_t id, const double value, const int direction) : id(id), value(value), direction(direction) {}

    bool operator< (const HeapElement& other) const {return this->value < other.value;}
};

/**
* This class is the implementation of a minimum, binary heap. We use this class during the hierarchy construction process as well as
* during bidirectional searches. Binary heaps support O(log(n)) insert and delete operations. In theory, the Fibonacci heap would be
* a better candidate for our purposes than the binary heap, as it supports O(1) insert operations. However, various research has shown that
* in practice a binary heap is actually faster (it is also much easier to implement).
*/
template<class T>
class Queue {

private:

    std::vector<T> heap_;

    void siftUp(int idx) {
        int parent = (idx - 1) / 2;

        while (idx > 0 && heap_[idx] < heap_[parent]) {
            std::swap(heap_[parent], heap_[idx]);
            idx = parent;
            parent = (parent - 1) / 2;
        }
    }

    void siftDown(int idx) {
        int left_child = 2 * idx + 1;
        int idx_to_swap, right_child;

        while (left_child < heap_.size()) {
            right_child = 2 * idx + 2;
            if (right_child < heap_.size() && heap_[right_child] < heap_[left_child]) {
                idx_to_swap = right_child;
            }
            else {
                idx_to_swap = left_child;
            }
            if (heap_[idx_to_swap] < heap_[idx]) {
                std::swap(heap_[idx],heap_[idx_to_swap]);
                idx = idx_to_swap;
                left_child = 2 * idx + 1;
            }
            else {
                return;
            }
        }
    }

public:

    // Takes a size parameter to allocate memory for the heap_ vector.
    explicit Queue(const int size = 0) { heap_.reserve(size); }

    // Gets the value of the item in the Queue with the highest priority. Returns a nullptr if the Queue is empty.
    const T& peek() {
        if (!heap_.empty()) {
            return heap_[0];
        } else {
            throw std::logic_error("Cannot peek an empty queue.");
        }
    }

    // Deletes an item from the Queue.
    T pop() {
        if (heap_.size() == 0) { throw std::logic_error("Cannot delete element in an empty queue."); }
        T minElement = heap_[0];
        std::swap(heap_[0], heap_.back());
        heap_.pop_back();
        siftDown(0);
        return minElement;
    }

    void lazyUpdate(T elem) {
        if (heap_.size() == 0) { throw std::logic_error("Cannot lazy update an empty queue."); }
        heap_[0] = elem;
        siftDown(0);
    }

    void clear() { heap_.clear(); }

    void makeHeap(const std::vector<T>& elems) {
        heap_ = elems;
        int i = heap_.size() / 2;
        while (i >= 0) {
            siftDown(i);
            i -= 1;
        }
    }

    // Insert an item in the Queue.
    void push(T elem) {
        heap_.push_back(elem);
        siftUp(heap_.size() - 1);
    }

    bool empty() const {
        if (heap_.empty()) { return true; }
        return false;
    }

    uint64_t size() const { return heap_.size(); }
};