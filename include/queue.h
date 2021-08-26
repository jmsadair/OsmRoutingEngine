#pragma once
#include <vector>
#include <unordered_map>
 
// This struct is used to store basic information about an item in the queue.
struct heap_element {
	
// This attribute is used during the bidirectional search. A 1 indicates forward search and a 0 indicates backward.
	int direction;

// Used to identify an item in the queue. Typically, this ID is the ID of an OSM node.
	unsigned long long id;

// Priority of an item in the queue. We implement a minimum, binary heap, so the lower this value is, the higher the priority of this item.
	double value;

	heap_element(unsigned long long id, double value);

	heap_element(unsigned long long id, double value, int direction);
};

/**
* This class is the implementation of a minimum, binary heap. We use this class during the hierarchy construction process as well as
* during bidirectional searches. Binary heaps support O(log(n)) insert and delete operations. In theory, the Fibonnaci heap would be 
* a better candidate for our purposes than the binary heap, as it supports O(1) insert operations. However, various research has shown that
* in practice a binary heap is actually faster (it is also much easier to implement). 
*/
class queue {

private:

	std::vector<heap_element> heap_;

// We use an unordered map to keep track of the indices of items in the queue. This is useful for increase / decrease key operations.
	std::unordered_map<unsigned long long, int> indices_;

	void sift_up(unsigned long long idx);

	void sift_down(unsigned long long idx);

public:
	
// Takes a size parameter to allocate memory for the heap_ vector.
	queue(unsigned long long size = 0);

	void make_heap(const std::vector<heap_element>& elems);

// Insert an item in the queue.
	void push(unsigned long long id, double value, int direction = -1);

	void decrease_key(unsigned long long id, double value);

	void increase_key(unsigned long long id, double value);

// Updates the value of the item in the queue with the highest priority. 
	void lazy_update(double value);

	void clear();

	bool is_empty()const;

// Gets the ID of the item in the queue with the highest priority. Returns -1 if the queue is empty.
	unsigned long long peek_id()const;

// Gets the direction of the item in the queue with the highest priority. Returns -1 if the queue is empty.
	int peek_direction()const;

	unsigned long long get_size()const;

// Gets the value of the item in the queue with the highest priority. Returns -1 if the queue is empty.
	double peek_value()const;

// Delete an item from the queue.
	unsigned long long pop();
};