#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "graph.h"
#include "queue.h"

/**
* The purpose of this class is to take a weighted, directed graph and modify it so that it can be used for optimal routing
* in road networks. The vertices in the graph are ordered by importance and then a hierarchy is generated by
* iteratively "contracting" the least important vertex. Contracting a vertex v means to 
* replace the shortest paths going through v by "shortucts".
* 
* NOTE: In general, we will use v to denote vertices that have an outgoing edge to the vertex being contracted i.e. v -> u, where
*       u is the vertex that is being contracted. We will use w to denote vertices that the contracted vertex has an 
*       outgoing edge to i.e. u -> w. 
* 
*		We will also use the term "witness path". Suppose that u is a vertex being contracted. A witness path from vertex v to 
*		vertex w is a path that does not contain u such that weight(path(v, w)) <= weight(v -> u -> w). 
*/
class constructor {

private:

	graph& graph_;

	long long total_edges_added_;

	std::unordered_map<unsigned long long, vertex> vertices_;


/**
* This method is used to contract a vertex. If simulated is set to true, no edges will be added. If simulated is set to 
* false, the neccesary shortcut edges will be added to the graph. The purpose of simulating the contraction of a vertex
* is to determine the cost of contracting the vertex. The cost of contracting a vertex is the number of shortcut edges 
* that must be added when we remove the vertex from the graph. 
*/
	int contract_vertex(const unsigned long long contracted_vertex, const int hop_limit, const bool simulated = false);

/**
* The purpose of this method is to find witness paths between vertices.  We find witness paths by applying a 
* standard unidirectional Dijkstra algorithm. The hop limit is the maximum number of vertices that we will allow to be 
* in the shortest path. If the hop limit is exceeded, we terminate the search. If u is the vertex being contracted, 
* then ehe maximum weight is weight(v, u) + max(weight(u, w)). We can abort the search if this weight is exceeded, because there
* is then no hope of finding a witness path. We can also abort the search if we have settled all outgoing vertices of the vertex being
* contracted. 
*/
	std::unordered_map<unsigned long long, double> witness_search(unsigned long long source, const unsigned long long contracted_vertex, double max_distance, int hop_limit);

/**
* This method updates the deleted neighbor counter of all vertices adjacent to the vertex being contracted.
* The deleted neighbor counter is used when determining the priority term of a vertex. The deleted neighbor counter
* ensures uniform contraction of nodes across the graph. Uniform contraction of nodes reduces preproccesing time and
* improves query time.
*/
	void contracted_neighbors(const unsigned long long contracted_vertex);

/**
* During the contraction of a node, the necessary shortcuts are gathered in a vector. This method adds those shortcuts
* to the graph.
*/
	void add_shortcuts(const unsigned long long contracted_vertex, const std::vector<std::tuple<unsigned long long, unsigned long long, double>>* shortcuts);

/**
* This method computes the edge difference when a vertex is contracted. The edge difference for a vertex u is given
* by the number of shortcuts that must be added when u is contracted minus the total number of incoming and outgoing edges
* that u has.
*/
	int get_edge_difference(const unsigned long long contracted_vertex, const int hop_limit);

	
// Determines the maximum outgoing edge weight of a vertex being contracted.
	double get_max_out_distance(const unsigned long long contracted_vertex) const;

/**
* This method gets the next vertex that is to be contracted. We check to see if the next vertex 
* in the queue still has the minimum cost by simulating its contraction. If the vertex no longer 
* has the minimum cost, the queue is updated and we again check to see if the next vertex in the 
* queue has the minimum cost. This process continues until we succesfully find a vertex that still 
* has the minimum cost after a simulated contraction.
*/
	unsigned long long get_next(const int hop_limit, queue* queue);
	
/**
* This method is used to compute the cost of contraction of all the vertices in the graph. A minimum binary heap is used
* to store the vertices and their associated cost.
*/
	queue get_initial_ordering(const int hop_limit);

// Simulates the contraction of a vertex and computes its cost of contraction.
	int get_priority_term(const unsigned long long contracted_vertex, const int hop_limit, const bool simulated = false);

public:

// We take a reference to the graph because we will be adding edges to it as the contraction process takes place.
	constructor(graph& graph);

// This method contracts all of the vertices in the graph and adds any necessary shortcuts. 
	void contract_graph();
};