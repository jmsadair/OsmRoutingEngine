#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Graph.h"
#include "Queue.h"

/**
* The purpose of this class is to take a weighted, directed graph and modify it so that it can be used for optimal routing
* in road networks. The vertices in the graph are ordered by importance and then a hierarchy is generated by
* iteratively "contracting" the least important Vertex. Contracting a Vertex v means to
* replace the shortest paths going through v by "shortcuts".
*
* NOTE: In general, we will use v to denote vertices that have an outgoing Edge to the Vertex being contracted i.e. v -> u, where
* u is the Vertex that is being contracted. We will use w to denote vertices that the contracted Vertex has an
* outgoing Edge to i.e. u -> w. We will also use the term "witness path". Suppose that u is a Vertex being contracted.
* A witness path from Vertex v to Vertex w is a path that does not contain u such that weight(path(v, w)) <= weight(v -> u -> w).
*/
class HierarchyConstructor {

private:
    // The hop limit is the maximum number of vertices that we will allow to be in the shortest path during a witness search.
    // If the hop limit is exceeded, we terminate the search.
    static const int HOP_LIMIT = 1000;

    // The graph that will be contracted.
    Graph& graph_;

    // The total number of edges added to the graph during the contraction process.
    int64_t total_edges_added_;

    // A multiplier for the edge difference priority term. Benchmarking has shown that 170 an ideal number.
    int edge_difference_coefficient;

    // A multiplier for the deleted neighbor priority term. Benchmarking has shown that 190 an ideal number.
    int deleted_neighbors_coefficient;

    std::unordered_map<uint64_t, Vertex> vertices_;

    /**
     * This method is used to contract a vertex during the hierarchy construction process.
     * @param contracted_vertex The ID of the vertex being contracted.
     * @param simulated If simulated is set to true, no edges will be added. If simulated is set to
     * false, the necessary shortcut edges will be added to the graph. The purpose of simulating the contraction of a Vertex
     * is to determine the cost of contracting the Vertex
     * @return An integer value that represents the cost of contracting this vertex. The cost of contracting a Vertex is the number of shortcut edges
     * that must be added when we remove the Vertex from the graph.
     */
    int contractVertex(uint64_t contracted_vertex, bool simulated = false);

    /**
     * The purpose of this method is to find witness paths between vertices. We find witness paths by applying a
     * standard unidirectional Dijkstra algorithm. If HOP_LIMIT is exceeded, we terminate the search. If u is the Vertex being contracted,
     * then the maximum weight is weight(v, u) + max(weight(u, w)). We can abort the search if this weight is exceeded, because there
     * is then no hope of finding a witness path. We can also abort the search if we have settled all outgoing vertices of the Vertex being
     * contracted.
     * @param source The ID of the vertex that is the starting point for the witness search.
     * @param contracted_vertex The ID of the vertex that is currently being contracted.
     * @param max_distance The maximum distance that we will allow a witness path to be before terminating the search.
     * @return An unordered map that maps vertex IDs to shortest path distances from the source vertex.
     */
    std::unordered_map<uint64_t, double> witnessSearch(uint64_t source, uint64_t contracted_vertex, double max_distance);

    /**
     * This method updates the deleted neighbor counter of all vertices adjacent to the Vertex being contracted.
     * The deleted neighbor counter is used when determining the priority term of a Vertex. The deleted neighbor counter
     * ensures uniform contraction of nodes across the graph. Uniform contraction of nodes reduces preprocessing time and
     * improves route query time.
     * @param contracted_vertex The ID of the vertex currently being contracted.
     */
    void contractedNeighbors(uint64_t contracted_vertex);

    /**
     * During the contraction of a node, the necessary shortcuts are gathered in a vector. This method adds those shortcuts
     * to the graph.
     * @param contracted_vertex The ID of the vertex currently being contracted.
     * @param shortcuts The shortcut edges that will be added to the graph.
     */
    void addShortcuts(uint64_t contracted_vertex, const std::vector<std::tuple<unsigned long long, unsigned long long, double>>* shortcuts);

    /**
     * This method computes the Edge difference when a Vertex is contracted. The Edge difference for a Vertex u is given
     * by the number of shortcuts that must be added when u is contracted minus the total number of incoming and outgoing edges
     * that u has.
     * @param contracted_vertex The ID of the vertex currently being contracted.
     * @return An integer representing the edge difference term.
     */
    int getEdgeDifference(uint64_t contracted_vertex);

    /**
     * Determines the maximum outgoing Edge weight of a Vertex being contracted. This distance is used for determining
     * the maximum possible length that a witness path can be.
     * @param contracted_vertex The vertex currently being contracted.
     * @return The maximum outgoing edge weight of the vertex currently being contracted.
     */
    double getMaxOutDistance(uint64_t contracted_vertex) const;

    /**
     * This method gets the next Vertex that is to be contracted. We check to see if the next Vertex
     * in the MinHeap still has the minimum cost by simulating its contraction. If the Vertex no longer
     * has the minimum cost, the MinHeap is updated and we again check to see if the next Vertex in the
     * MinHeap has the minimum cost. This process continues until we successfully find a Vertex that still
     * has the minimum cost after a simulated contraction.
     * @param queue A minimum binary heap that contains the vertices that must still be contracted.
     * @return The ID of the vertex that will be contracted next.
     */
    uint64_t getNext(Queue::MinHeap<HeapElement> *queue);

    /**
     * This method is used to compute the initial cost of contraction of all the vertices in the graph. A minimum binary heap is used
     * to store the vertices and their associated cost.
     * @return A minimum binary heap containing the vertices that will be contracted.
     */
    Queue::MinHeap<HeapElement> getInitialOrdering();

    /**
     * Computes its cost of contracting a given vertex.
     * @param contracted_vertex The ID of the vertex that will be contracted.
     * @param simulated If simulated is set to true, the vertex will not actually be contracted.
     * @return An integer representing the cost of contracting the vertex.
     */
    int getPriorityTerm(uint64_t contracted_vertex, bool simulated = false);

public:
    /**
     * A constructor for the HierarchyConstructor class.
     * @param graph A reference to the graph that will be contracted.
     * @param edge_difference_coefficient A multiplier that will be used when computing the cost of contracting a vertex.
     * @param deleted_neighbors_coefficient A multiplier that will be used when computing the cost of contracting a vertex.
     */
    explicit HierarchyConstructor(Graph& graph, int edge_difference_coefficient = 170, int deleted_neighbors_coefficient = 190);

    /**
     * Contracts all the vertices in the provided graph. Note that this mutates the graph.
     */
    void contractGraph();
};