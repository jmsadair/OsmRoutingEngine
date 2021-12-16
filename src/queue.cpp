#include <string>
#include "queue.h"

heap_element::heap_element(const uint64_t id, const double value) : id(id), value(value), direction(-1) {}

heap_element::heap_element(const uint64_t id, const double value, const int direction) : id(id), value(value), direction(direction) {}

queue::queue(const int size) { heap_.reserve(size); }

void queue::sift_up(int idx) {
	int parent = (idx - 1) / 2;

	while (idx > 0 && heap_[idx].value < heap_[parent].value) {
		indices_[heap_[idx].id] = parent;
		indices_[heap_[parent].id] = idx;
		std::swap(heap_[parent], heap_[idx]);
		idx = parent;
		parent = (parent - 1) / 2;
	}
}

void queue::sift_down(int idx) {
	int left_child = 2 * idx + 1;
    int idx_to_swap, right_child;

    while (left_child < heap_.size()) {
        right_child = 2 * idx + 2;
        if (right_child < heap_.size() && heap_[right_child].value < heap_[left_child].value) {
            idx_to_swap = right_child;
        }
        else {
            idx_to_swap = left_child;
        }
        if (heap_[idx_to_swap].value < heap_[idx].value) {
            indices_[heap_[idx].id] = idx_to_swap;
            indices_[heap_[idx_to_swap].id] = idx;
            std::swap(heap_[idx],heap_[idx_to_swap]);
            idx = idx_to_swap;
            left_child = 2 * idx + 1;
        }
        else {
            return;
        }
    }
}

void queue::make_heap(const std::vector<heap_element>& elems) {
	heap_ = elems;
	int i = heap_.size() / 2;
	while (i >= 0) {
		sift_down(i);
		i -= 1;
	}
}

void queue::push(const uint64_t id, const double value, const int direction) {
	if (direction == 1 || direction == 0) {
		heap_.emplace_back(id, value, direction);
	}
	else {
		heap_.emplace_back(id, value);
	}
	indices_[id] = heap_.size() - 1;
	sift_up(heap_.size() - 1);
}

uint64_t queue::pop() {
	const uint64_t min_element = heap_[0].id;

	indices_.erase(min_element);
	indices_[heap_.back().id] = 0;
	std::swap(heap_[0], heap_.back());
	heap_.pop_back();
	sift_down(0);

	return min_element;
}

void queue::decrease_key(const uint64_t id, const double value) {
	heap_[indices_[id]].value = value;
	sift_up(indices_[id]);
}

void queue::increase_key(const uint64_t id, const double value) {
	heap_[indices_[id]].value = value;
	sift_down(indices_[id]);
}

void queue::lazy_update(const double value) {
	heap_[0].value = value;
	sift_down(0);
}

void queue::clear() {
	heap_.clear();
	indices_.clear();
}

bool queue::is_empty() const {
	if (heap_.empty()) { return true; }
    return false;
}

uint64_t queue::peek_id() const {
	if (!is_empty()) { return heap_[0].id; }
    return -1;
}

int queue::peek_direction() const {
	if (!is_empty()) { return heap_[0].direction; }
    return -1;
}

uint64_t queue::get_size() const {
	return heap_.size();
}

double queue::peek_value() const {
	if (!is_empty()) { return heap_[0].value; }
    return -1;
}




