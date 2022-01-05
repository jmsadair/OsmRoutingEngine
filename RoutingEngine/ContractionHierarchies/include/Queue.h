#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

// This struct is used to store basic information about an item in the MinHeap.
struct HeapElement {

    // This attribute is used during the bidirectional search. A 1 indicates forward search and a 0 indicates backward.
    int direction;

    // Used to identify an item in the MinHeap. Typically, this ID is the ID of an OSM node.
    uint64_t id;

    // Priority of an item in the MinHeap.
    double value;

    /**
     * A constructor for the HeapElement struct.
     * @param id The ID that will be used for the HeapElement.
     * @param value The value of the HeapElement. Serves as the key in the heap.
     */
    HeapElement(const uint64_t id, const double value) : id(id), value(value), direction(-1) {}

    /**
     * A constructor for the HeapElement struct.
     * @param id The ID that will be used for the HeapElement.
     * @param value The value of the HeapElement. Serves as the key in the heap.
     * @param direction A 1 or a 0 to indicate what direction of a bidirectional search this corresponds to. A 1 indicates forward
     * and a 0 indicates backwards.
     */
    HeapElement(const uint64_t id, const double value, const int direction) : id(id), value(value), direction(direction) {}

    /**
     * A comparator for the HeapElement struct.
     * @param other The HeapElement that this HeapElement is being compared to.
     * @return Returns true if this HeapElement's value attribute is less than the other HeapElement's value attribute.
     */
    bool operator< (const HeapElement& other) const {return this->value < other.value;}
};

/**
* This class is the implementation of a minimum, binary heap. We use this class during the hierarchy construction process as well as
* during bidirectional searches. Binary heaps support O(log(n)) insert and delete operations. In theory, the Fibonacci heap would be
* a better candidate for our purposes than the binary heap, as it supports O(1) insert operations. However, various research has shown that
* in practice a binary heap is actually faster (it is also much easier to implement).
*/
namespace Queue {
    template<class T>
    class MinHeap {

    private:
        // A vector used to store the elements of the heap.
        std::vector<T> heap_;

        /**
         * This method is used to preserve the heap property of the heap after a new element is inserted into the heap.
         * An element is initially inserted as a leaf in the heap tree. If the newly inserted element's key is less than
         * its parent key, then it must be swapped with its parent. This process is repeated until the parent key is not
         * less than the newly inserted element's key or until it has no parent.
         * @param idx The index of the newly inserted heap element.
         */
        void siftUp(int idx) {
            int parent = (idx - 1) / 2;

            while (idx > 0 && heap_[idx] < heap_[parent]) {
                std::swap(heap_[parent], heap_[idx]);
                idx = parent;
                parent = (parent - 1) / 2;
            }
        }

        /**
         * This method is used to preserve the heap property of the heap after deleting an element from the heap or lazy
         * updating the heap. If the first element in the heap is not the minimum element in the heap, then the first element
         * will be swapped with its child elements until it is a leave or until its children are of greater value.
         * @param idx The index of the heap element that will be sifted down.
         */
        void siftDown(int idx) {
            int left_child = 2 * idx + 1;
            int idx_to_swap, right_child;

            while (left_child < heap_.size()) {
                right_child = 2 * idx + 2;
                if (right_child < heap_.size() && heap_[right_child] < heap_[left_child]) {
                    idx_to_swap = right_child;
                } else {
                    idx_to_swap = left_child;
                }
                if (heap_[idx_to_swap] < heap_[idx]) {
                    std::swap(heap_[idx], heap_[idx_to_swap]);
                    idx = idx_to_swap;
                    left_child = 2 * idx + 1;
                } else {
                    return;
                }
            }
        }

    public:

        /**
         * A constructor for the MinHeap class.
         * @param size The expected size of the heap. Used to reserve memory for the vector that will contain the
         * heap elements
         */
        explicit MinHeap(const int size = 0) { heap_.reserve(size); }

        /**
         * Retrieves the minimum element in the heap without deleting it.
         * @return The minimum element in the heap.
         */
        const T &peek() {
            if (!heap_.empty()) {
                return heap_[0];
            } else {
                throw std::logic_error("Cannot peek an empty queue.");
            }
        }

        /**
         * Deletes the minimum element from the heap and updates the heap such that heap property is preserved.
         * @return The minimum element in the heap.
         */
        T pop() {
            if (heap_.size() == 0) { throw std::logic_error("Cannot delete element in an empty queue."); }
            T minElement = heap_[0];
            std::swap(heap_[0], heap_.back());
            heap_.pop_back();
            siftDown(0);
            return minElement;
        }

        /**
         * Updates the value of the minimum value in the heap.
         * @param elem The new value of the minimum element in the heap.
         */
        void lazyUpdate(T elem) {
            if (heap_.size() == 0) { throw std::logic_error("Cannot lazy update an empty queue."); }
            heap_[0] = elem;
            siftDown(0);
        }

        /**
         * Deletes all the elements in the heap.
         */
        void clear() { heap_.clear(); }

        /**
         * Given a vector of elements, creates a heap from the vector.
         * @param elems The vector of elements that will be transformed into a heap.
         */
        void makeHeap(const std::vector<T> &elems) {
            heap_ = elems;
            int i = heap_.size() / 2;
            while (i >= 0) {
                siftDown(i);
                i -= 1;
            }
        }

        /**
         * Inserts a new element into the heap.
         * @param elem The element that is to be inserted into the heap.
         */
        void push(T elem) {
            heap_.push_back(elem);
            siftUp(heap_.size() - 1);
        }

        /**
         * Indicates whether the heap is empty or not.
         * @return Returns true if the heap is empty, otherwise false.
         */
        [[nodiscard]] bool empty() const {
            return heap_.empty();
        }

        /**
         * Retrieves the size of the heap.
         * @return The size of the heap.
         */
        [[nodiscard]] uint64_t size() const { return heap_.size(); }
    };
}