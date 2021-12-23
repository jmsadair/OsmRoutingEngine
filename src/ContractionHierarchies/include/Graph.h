#pragma once
#include <vector>
#include <unordered_map>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <sstream>


/**
* This struct stores basic information about a Vertex as well as adjacent vertices
* and their weights.
*/
struct Vertex {

    // A standard 64-bit OSM node id.
    uint64_t id;

    // Adjacent vertices and their Edge weights.
    std::unordered_map<uint64_t, double> in_edges, out_edges;

    // Keeps track of how many adjacent vertices have been contracted. Used when computing priority term in hierarchy construction.
    int deleted_neighbors;

    // The importance of a Vertex. Used in the bidirectional search.
    uint64_t order;

    explicit Vertex(uint64_t id, uint64_t order = 0, bool contracted = false);

    Vertex();

    // Serializes necessary information so that the graph can be saved in a binary file.
    template <class Archive>
    void serialize(Archive& ar) { ar(id, in_edges, out_edges, order); }
};

/**
* The Edge struct is primarily used to store the OSM nodes that connect two vertices in the graph.
* All of these edges are added during OSM file parsing. These edges play no role in the hierarchy
* construction and the bidirectional search, but they should be added to a path if it is to be
* plotted on a map.
*/
struct Edge {

    // The OSM nodes that make up an Edge in a graph.
    std::vector<uint64_t> nodes;

    // Note that the start and end nodes are not actually stored in the nodes vector.
    uint64_t start;
    uint64_t end;

    // The weight of the Edge (generally, we use a time weight in minutes).
    double weight;

    Edge(uint64_t start, uint64_t end, std::vector<uint64_t>& nodes, double weight);

    Edge(uint64_t start, uint64_t end, double weight);

    Edge();

    // Serializes necessary information so that the graph can be saved in a binary file.
    template <class Archive>
    void serialize(Archive& ar) { ar(start, end, nodes, weight); }
};

/**
* This class represents a standard, weighted, directed graph. For routing, the vertices of the graph represent intersections. There is an Edge
* connecting two vertices if the intersections they represent are adjacent to one another. If a road is one way, only one Edge will be added
* between vertices. If a road is two way, two edges will be added. The edges are assigned a weight in minutes. This is the travel time from
* one intersection to another.
*/
class Graph {

private:

    const double INF_ = std::numeric_limits<double>::infinity();

    uint64_t num_edges_;

    // Keeps track of the shortcuts that are added to the graph during contraction. Used when unpacking the shortest paths in bidirectional search.
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> shortcuts_;

    std::unordered_map<uint64_t, std::unordered_map<uint64_t, Edge>> edges_;

    std::unordered_map<uint64_t, Vertex> vertices_;

public:

    Graph();

    /**
    * This method is primarily used when parsing the OSM data. Adds an Edge to the graph and stores the nodes that
    * make up the Edge. If the start or end Vertex is not in the graph, it will be added. This method should NOT
    * be used when adding a shortcut Edge.
    */
    void addEdge(uint64_t start, uint64_t end, std::vector<uint64_t> *nodes, double weight = 0.0, bool bidirectional = false);
    void addEdge(uint64_t start, uint64_t end, double weight = 0.0, bool bidirectional = false);

    // Checks whether a given Edge is present in the graph or not (checks shortcut edges too).
    bool edgeExists(uint64_t start, uint64_t end);

    void removeEdge(uint64_t start, uint64_t end);

    /**
    * This method is used during the contraction of the graph. Adds the Edge and records the shortcut for later use
    * in routing.
    */
    void addShortcut(uint64_t start, uint64_t end, uint64_t through, double weight);

    // Updates the importance of a Vertex during hierarchy construction.
    void addOrdering(uint64_t vertex, uint64_t ordering);

    uint64_t getNumVertices() const { return vertices_.size(); }

    uint64_t getNumEdges() const { return num_edges_; }

    std::unordered_map<uint64_t, Vertex> getVertices() const { return vertices_; }

    /**
    * Computes the shortest path using a modified bidirectional search algorithm. If standard is set to true, a standard bidirectional Dijkstra search is
    * conducted instead. The standard bidirectional Dijkstra search is only used for testing, as it much slower than the modified bidirectional search.
    */
    std::pair<std::vector<uint64_t>, double> getShortestPath(uint64_t source, uint64_t target, bool standard = false);

    /**
    * Used to remove unnecessary edges after the graph is contracted. Edges that start at a Vertex of greater order than the Vertex the Edge ends at can
    * be removed from the graph because these edges will never appear on the shortest path.
    */
    void optimizeEdges();

    // Used for saving the graph to a binary file.
    template <class Archive>
    void save(Archive& ar) const{ ar(vertices_, edges_, shortcuts_); }

    // Used for loading a graph stored in a binary file.
    template <class Archive>
    void load(Archive& ar) { ar(vertices_, edges_, shortcuts_); }
};

