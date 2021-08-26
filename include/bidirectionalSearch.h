#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "queue.h"
#include "graph.h"

/**
* The purpose of this class is to find the shortest path between two vertices in a graph. We implement two different
* search algorithms: the standard, bidirectional Dijkstra algorithm and a modified bidirectional search. 
*/
class bidirectional_search {

private:

	const double INF_ = std::numeric_limits<double>::infinity();

// We need to to see the vertices in the graph in order to find the shortest path.
	const std::unordered_map<unsigned long long, vertex>* vertices_;

// We need to see the shortcuts to unpack the shortest path.
	const std::unordered_map<unsigned long long, std::unordered_map<unsigned long long, unsigned long long>>* shortcuts_;

// Keeps track of which vertices have been settled in the forward and reverse search. 
	std::unordered_set<unsigned long long> visited_source_, visited_target_;

// Keeps track of the length of the shortest path found so far for each vertex encountered in the search.
	std::unordered_map<unsigned long long, double> dist_source_, dist_target_;

// Keeps track of the order in which the vertices are visited. Used for reconstructing the shortest path after the search is terminated.
	std::unordered_map<unsigned long long, long long> prev_source_, prev_target_;

/**
* A standard, minimum, binary heap is used as the priority queue for the search. Insetad of using two priority queues for the bidirectional search,
* we use one. Each item in the queue stores the direction it represents. A 1 indicates that a forward search should be performed when relaxing that
* that vertices edges and a 0 indicates a backwards search. 
*/
	queue queue_;

/**
* This method will be used during the the bidirectional searches. The process of relaxing an edge (u,v) consists of testing whether we can
* improve the shortest path to v found so far by going through u and, if so, updating dist_source[v] /dist_target[v] and prev_source[v] / prev_target[v].
* A relaxtion step may decrease the value of the shortest path estimate dist_source[v] / dist_target[v] and update v's predecessors.
*/
	void relax_edge(const unsigned long long vertex, const bool backward = false, const bool alt_search = false);

// Using the predecessors of the vertices settled in the bidirectional searches, we reconstruct the shortest path. 
	std::vector<unsigned long long> reconstruct_path(unsigned long long source, unsigned long long target, unsigned long long intersection);

// Shortcut edges are added to the graph during the contraction process. We need to add the vertices that these shortcuts go through to the path.
	std::vector<unsigned long long> unpack_shortcut(unsigned long long start, unsigned long long end);

// Unpacks all of the shortucts in the path.
	std::vector<unsigned long long> unpack_path(std::vector<unsigned long long>* path);

// Checks whether a shortuct exists between two vertices. 
	bool is_shortcut(const unsigned long long start, const unsigned long long end);
	

public:

	bidirectional_search(const std::unordered_map<unsigned long long, vertex>* vertices, const std::unordered_map<unsigned long long, std::unordered_map<unsigned long long, unsigned long long>>* shortcuts);

/**
* This is the primary search we will use for routing. In order to use this method, the graph must first be contracted. This seach is similar to a bidirectional Dijkstra search,
* but instead of relaxing all edges that are adjacent to a vertex that is popped from the queue, we only relax edges that lead to nodes with higher order than that vertex.
* In turn, this search is much faster than a bidirectional Dijkstra search. Returns a pair containing the shortest path and the lenght of the shortest path.
*/
	std::pair<std::vector<unsigned long long>, double> execute_search(unsigned long long source, unsigned long long target);

// This is just a standard, bidirectional Dijkstra search. Primarily used for testing. Returns a pair containing the shortest path and the lenght of the shortest path.
	std::pair<std::vector<unsigned long long>, double> execute_standard_search(unsigned long long source, unsigned long long target);
};