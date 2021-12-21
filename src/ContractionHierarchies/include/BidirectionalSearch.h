#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Queue.h"
#include "Graph.h"

/**
* The purpose of this class is to find the shortest path between two vertices in a graph. We implement two different
* search algorithms: the standard, bidirectional Dijkstra algorithm and a modified bidirectional search.
*/
class BidirectionalSearch {

private:

    const double INF_ = std::numeric_limits<double>::infinity();

    // We need to see the vertices in the graph in order to find the shortest path.
    const std::unordered_map<uint64_t, Vertex>* vertices_;

    // We need to see the shortcuts to unpack the shortest path.
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>* shortcuts_;

    // Keeps track of which vertices have been settled in the forward and reverse search.
    std::unordered_set<uint64_t> visited_source_, visited_target_;

    // Keeps track of the length of the shortest path found so far for each Vertex encountered in the search.
    std::unordered_map<uint64_t, double> dist_source_, dist_target_;

    // Keeps track of the order in which the vertices are visited. Used for reconstructing the shortest path after the search is terminated.
    std::unordered_map<uint64_t, uint64_t> prev_source_, prev_target_;

    /**
    * A standard, minimum, binary heap is used as the priority Queue for the search. Instead of using two priority queues for the bidirectional search,
    * we use one. Each item in the Queue stores the direction it represents. A 1 indicates that a forward search should be performed when relaxing that
    * that vertices edges and a 0 indicates a backwards search.
    */
    Queue<HeapElement> queue_;

    /**
    * This method will be used during the the bidirectional searches. The process of relaxing an Edge (u,v) consists of testing whether we can
    * improve the shortest path to v found so far by going through u and, if so, updating dist_source[v] /dist_target[v] and prev_source[v] / prev_target[v].
    * A relaxation step may decrease the value of the shortest path estimate dist_source[v] / dist_target[v] and update v's predecessors.
    */
    void relax_edge(uint64_t vertex, bool backward = false, bool alt_search = false);

    // Using the predecessors of the vertices settled in the bidirectional searches, we reconstruct the shortest path.
    std::vector<uint64_t> reconstruct_path(uint64_t source, uint64_t target, uint64_t intersection);

    // Shortcut edges are added to the graph during the contraction process. We need to add the vertices that these shortcuts go through to the path.
    std::vector<uint64_t> unpack_shortcut(uint64_t start, uint64_t end);

    // Unpacks all the shortcuts in the path.
    std::vector<uint64_t> unpack_path(const std::vector<uint64_t>* path);

    // Checks whether a shortcut exists between two vertices.
    bool is_shortcut(uint64_t start, uint64_t end);


public:

    BidirectionalSearch(const std::unordered_map<uint64_t, Vertex>* vertices, const std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>* shortcuts);

    /**
    * This is the primary search we will use for routing. In order to use this method, the graph must first be contracted. This search is similar to a bidirectional Dijkstra search,
    * but instead of relaxing all edges that are adjacent to a Vertex that is popped from the Queue, we only relax edges that lead to nodes with higher order than that Vertex.
    * In turn, this search is much faster than a bidirectional Dijkstra search. Returns a pair containing the shortest path and the length of the shortest path.
    */
    std::pair<std::vector<uint64_t>, double> execute_search(uint64_t source, uint64_t target);

    // This is just a standard, bidirectional Dijkstra search. Primarily used for testing. Returns a pair containing the shortest path and the length of the shortest path.
    std::pair<std::vector<uint64_t>, double> execute_standard_search(uint64_t source, uint64_t target);
};