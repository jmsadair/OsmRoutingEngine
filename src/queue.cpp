#include <string>
#include <iostream>
#include "queue.h"

heap_element::heap_element(const unsigned long long id, const double value)
	: id(id), value(value), direction(-1)
{}


heap_element::heap_element(const unsigned long long id, const double value, const int direction)
	: id(id), value(value), direction(direction)
{}

queue::queue(const unsigned long long size) {

	heap_.reserve(size);
}

void queue::sift_up(unsigned long long idx) {

	unsigned long long parent = idx / 2;

	while (heap_[idx].value < heap_[parent].value) {

		indices_[heap_[idx].id] = parent;
		indices_[heap_[parent].id] = idx;
		std::swap(heap_[parent], heap_[idx]);
		idx = parent;
		parent = parent / 2;
	}
}

void queue::sift_down(const unsigned long long idx) {

	const unsigned long long left = 2 * idx + 1;
	const unsigned long long right = 2 * idx + 2;
	unsigned long long smallest = idx;

	if (left < heap_.size() && heap_[left].value < heap_[idx].value) {

		smallest = left;
	}

	if (right < heap_.size() && heap_[right].value < heap_[smallest].value) {

		smallest = right;
	}

	if (smallest != idx) {

		indices_[heap_[idx].id] = smallest;
		indices_[heap_[smallest].id] = idx;
		std::swap(heap_[idx], heap_[smallest]);
		sift_down(smallest);
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

void queue::push(unsigned long long id, double value, int direction) {

	if (direction == 1 || direction == 0) {

		heap_.emplace_back(id, value, direction);
	}
	else {

		heap_.emplace_back(id, value);
	}

	indices_[id] = heap_.size() - 1;
	sift_up(heap_.size() - 1);
}

unsigned long long queue::pop() {

	const unsigned long long min_element = heap_[0].id;

	indices_.erase(min_element);
	indices_[heap_.back().id] = 0;
	heap_[0] = heap_.back();
	heap_.pop_back();
	sift_down(0);

	return min_element;
}

void queue::decrease_key(const unsigned long long id, const double value) {

	heap_[indices_[id]].value = value;
	sift_up(indices_[id]);
}

void queue::increase_key(const unsigned long long id, const double value) {

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

	if (heap_.empty()) {

		return true;
	}
	else {

		return false;
	}
}

unsigned long long queue::peek_id() const {
	
	if (!is_empty()) {

		return heap_[0].id;
	}
	else {

		return -1;
	}
}

int queue::peek_direction() const {

	if (!is_empty()) {

		return heap_[0].direction;
	}
	else {

		return -1;
	}
}

unsigned long long queue::get_size() const {

	return heap_.size();
}

double queue::peek_value() const {

	if (!is_empty()) {

		return heap_[0].value;
	}
	else {

		return -1;
	}
}




