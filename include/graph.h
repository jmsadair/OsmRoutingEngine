#pragma once
#include <vector>
#include <unordered_map>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

/**
* This struct stores basic infomration about a vertex as well as adjacent vertices
* and their weights. 
*/
struct vertex {

// A standard 64-bit OSM node id.
	unsigned long long id;

// Adjacent vertices and their edge weights.
	std::unordered_map<unsigned long long, double> in_edges, out_edges;
	
// Keeps track of how many adjacent vertices have been contracted. Used when computing priority term in hierarchy construction.
	int deleted_neighbors;

// The importance of a vertex. Used in the bidirectional search.
	unsigned long long order;

	vertex(unsigned long long id, unsigned long long order = 0, bool contracted = false);

	vertex();

// Serializes necessary information so that the graoh can be saved in a binary file.
	template <class Archive>
	void serialize(Archive& ar) {

		ar(id, in_edges, out_edges, order);
	}
};

/**
* The edge struct is primarily used to store the OSM nodes that connect two vertices in the graph.
* All of these edges are added during OSM file parsing. These edges play no role in the hiearchy 
* construction and the bidirectional search, but they should be added to a path if it is to be
* plotted on a map. 
*/
struct edge {

// The OSM nodes that make up an edge in a graph.
	std::vector<unsigned long long> nodes;

// Note that the start and end nodes are not actually stored in the nodes vector.
	unsigned long long start, end;

// The weight of the edge (generally, we use a time weight in minutes).
	double weight;

	edge(unsigned long long start, unsigned long long end, std::vector<unsigned long long>& nodes, double weight);

	edge();

// Serializes necessary information so that the graoh can be saved in a binary file.
	template <class Archive>
	void serialize(Archive& ar) {

		ar(start, end, nodes, weight);
	}
};

/**
* This class represents a standard, weighted, directed graph. For routing, the vertices of the graph represent intersections. There is an edge
* connecting two vertices if the intersections they represent are adjacent to one another. If a road is one way, only one edge will be added
* between vertices. If a road is two way, two edges will be added. The edges are assigned a weight in minutes. This is the travel time from 
* one intersection to another.
*/
class graph {

private:

	const double INF_ = std::numeric_limits<double>::infinity();

	unsigned long long num_edges_;

// Keeps track of the shortucts that are added to the graoh during contraction. Used when unpacking shortest paths in bidirectional search.
	std::unordered_map<unsigned long long, std::unordered_map<unsigned long long, unsigned long long>> shortcuts;

	std::unordered_map<unsigned long long, std::unordered_map<unsigned long long, edge>> edges;

	std::unordered_map<unsigned long long, vertex> vertices;

public:

	graph();

/**
* This method is primarily used when parsing the OSM data. Adds an edge to the graph and stores the nodes that
* make up the edge. If the start or end vertex is not in the graph, it will be added. This method should NOT
* be useed when adding a shortcut edge.
*/
	void add_edge(const unsigned long long start, const unsigned long long end, std::vector<unsigned long long>& nodes, const double weight, const bool bidirectional = false);

// Checks whether a given edge is present in the graph or not (checks shortcut edges too).
	bool edge_exists(const unsigned long long start, const unsigned long long end);

	void remove_edge(unsigned long long start, unsigned long long end);

/**
* This method is used during the contraction of the graph. Adds the edge and records the shortcut for later use
* in routing.
*/
	void add_shortcut(unsigned long long start, unsigned long long end, unsigned long long through, double weight);

// Updates the importance of a vertex during hierarchy construction.
	void add_ordering(unsigned long long vertex, unsigned long long ordering);

	unsigned long long get_num_vertices() const { return vertices.size(); }

	unsigned long long get_num_edges() const { return num_edges_; }

	std::unordered_map<unsigned long long, vertex> get_vertices() const { return vertices; }

/**
* Computes the shortest path using a modified bidirectional search algorithm. If standard is set to true, a standard bidirectional Dijkstra search is
* conducted instead. The standard bidirectional Dijkstra search is only used for testing, as it much slower than the modified bidirectional search.
*/
	std::pair<std::vector<unsigned long long>, double> get_shortest_path(unsigned long long source, unsigned long long target, const bool standard = false);

/**
* Used to remove unnecessary edges after the graph is contracted. Edges that start at a vertex of greater order than the vertex the edge ends at can
* be removed from the graph becuase these edges will never appear on the shortest path. 
*/
	void optimize_edges();
	

	// Used for saving the graph to a binary file.
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(vertices, edges, shortcuts);
	}

	// Used for loading a graph stored in a binary file.
	template <class Archive>
	void load(Archive& ar)
	{
		ar(vertices, edges, shortcuts);
	}
};

