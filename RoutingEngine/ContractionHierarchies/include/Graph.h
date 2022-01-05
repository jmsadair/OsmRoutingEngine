#pragma once
#include <vector>
#include <unordered_map>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>
#include <fstream>
#include <sstream>

/**
* This struct stores basic information about a Vertex as well as adjacent vertices
* and their weights.
*/
struct Vertex {

    // A standard 64-bit OSM node id.
    uint64_t id;

    // Adjacent vertices and their edge weights.
    std::unordered_map<uint64_t, double> in_edges, out_edges;

    // Keeps track of how many adjacent vertices have been contracted. Used when computing priority term in hierarchy
    // construction.
    int deleted_neighbors;

    // The importance of a Vertex. Used in the bidirectional search.
    uint64_t order;

    /**
     * A constructor for the Vertex struct.
     * @param id The ID of the vertex.
     * @param order The order of the vertex.
     * @param contracted A boolean value indicating whether the vertex has been contracted or not.
     */
    explicit Vertex(uint64_t id, uint64_t order = 0, bool contracted = false);

    Vertex();

    /**
     * Serializes necessary information so that the graph can be saved in a binary file. See cereal documentation.
     * @tparam Archive See cereal documentation.
     * @param ar See cereal documentation.
     */
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

    // The weight of the edge in time (generally in minutes).
    double time_weight;

    // The weight of an edge in distance (generally in miles).
    double distance_weight;

    // This weight is primarily used for testing purposes. Represents an arbitrary edge weight.
    double weight;

    /**
     * A constructor for the Edge struct.
     * @param start The vertex ID on one end of the edge.
     * @param end The vertex ID on the other end of the edge.
     * @param nodes The OSM Node IDs that make up the edge.
     * @param time_weight The weight of the edge in time units.
     * @param distance_weight The weight of the edge in distance units.
     */

    Edge(uint64_t start, uint64_t end, std::vector<uint64_t>& nodes, double time_weight, double distance_weight);

    /**
     * A constructor for the Edge struct. This constructor is primarily used for testing purposes.
     * @param start The vertex ID on one end of the edge.
     * @param end The vertex ID on the other end of the edge.
     * @param weight An edge weight.
     */
    Edge(uint64_t start, uint64_t end, double weight);

    Edge();

    /**
     * Serializes necessary information so that the graph can be saved in a binary file. See cereal documentation.
     * @tparam Archive See cereal documentation.
     * @param ar See cereal documentation.
    */
    template <class Archive>
    void serialize(Archive& ar) { ar(start, end, nodes, time_weight, distance_weight); }
};

/**
 * This class represents a standard, weighted, directed graph. For routing, the vertices of the graph represent
 * intersections. There is an Edge connecting two vertices if the intersections they represent are adjacent to one
 * another. If a road is one way, only one Edge will be added between vertices. If a road is two way, two edges will be
 * added. The edges are assigned a weight in minutes. This is the travel time from one intersection to another.
*/
class Graph {

private:

    // Number of edges in the graph. Accounts for shortcut edges as well.
    uint64_t num_edges_;

    // Keeps track of the shortcuts that are added to the graph during contraction. Used when unpacking the shortest
    // paths in bidirectional search.
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> shortcuts_;

    // Maps vertex IDs to coordinates.
    std::unordered_map<uint64_t, std::array<double, 2>> locations_;

    // Keeps track of the edges present in the graph (these edges represent road segments). Lookup an edge by using a
    // start and end node ID as keys to the hash tables.
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, Edge>> edges_;

    // Maps vertex IDs to Vertex objects.
    std::unordered_map<uint64_t, Vertex> vertices_;

public:

    /**
     * A constructor for the Graph class.
     * @param locations An unordered map that maps vertex IDs to arrays containing longitude and latitude. Primarily
     * used for converting a path containing vertex IDs to a path containing coordinates.
     */
    explicit Graph(std::unordered_map<uint64_t, std::array<double, 2>> locations);

    Graph();

    /**
     * This method is primarily used when parsing the OSM data. Adds an Edge to the graph and stores the nodes that
     * make up the Edge. If the start or end vertex is not in the graph, it will be added. This method should NOT
     * be used when adding a shortcut Edge.
     * @param start The ID of the vertex on one end of the edge.
     * @param end The ID of the vertex on the other end of the edge.
     * @param nodes The OSM node IDs that connect the start and end vertices.
     * @param time_weight The time unit weight of the edge.
     * @param distance_weight The distance unit weight of the edge.
     * @param bidirectional A boolean value indicating whether a bidirectional edge should be added to the graph.
     * @param time A boolean value indicating whether a time weight should be the primary weight of the edge. This is
     * the weight that will be used when computing routes.
     */
    void addEdge(uint64_t start, uint64_t end, std::vector<uint64_t> *nodes, double time_weight, double distance_weight,
                 bool bidirectional = false, bool time = true);

    /**
     * This method adds an edge to the graph, but does not store the OSM nodes that make up the edge. Primarily used for
     * testing.
     * @param start The ID of the vertex on one end of the edge.
     * @param end The ID of the vertex on the other end of the edge.
     * @param weight The weight of the edge to be added.
     * @param bidirectional A boolean value indicating whether a bidirectional edge should be added to the graph.
     */
    void addEdge(uint64_t start, uint64_t end, double weight = 0.0, bool bidirectional = false);

    /**
     * Checks whether a given Edge is present in the graph or not (checks shortcut edges too).
     * @param start The ID of the vertex on one end of the edge (if the edge exist).
     * @param end The ID of the vertex on the other end of the edge (if the edge exist).
     * @return A boolean indicating whether the edge is present in the graph or not.
     */
    bool edgeExists(uint64_t start, uint64_t end);

    // Deletes an edge from the graph if it exists. Otherwise, raises an exception.
    void removeEdge(uint64_t start, uint64_t end);


    /**
     * This method is used during the contraction of the graph. Adds a shortcut edge to the graph and records the
     * shortcut for later use in routing.
     * @param start The ID of the first vertex on the edge.
     * @param end The ID of the second vertex on the edge.
     * @param through The ID of the vertex that the shortcut edge goes through.
     * @param weight The weight of the shortcut edge.
     */
    void addShortcut(uint64_t start, uint64_t end, uint64_t through, double weight);

    /**
     * Updates the importance of a Vertex during hierarchy construction.
     * @param vertex The ID of the vertex to be updated.
     * @param ordering The new ordering of the vertex.
     */
    void addOrdering(uint64_t vertex, uint64_t ordering);

    /**
     * This method gets the number of vertices present in the graph.
     * @return an integer that denotes how many vertices are in the graph.
     */
    uint64_t getNumVertices() const { return vertices_.size(); }

    /**
     * The method gets the number of edges present in the graph, including any shortcut edges.
     * @return an integer that denotes how many edges are in the graph.
     */
    uint64_t getNumEdges() const { return num_edges_; }

    /**
     * This method gets a copy of the vertices in the graph.
     * @return A hashmap that maps vertex IDs to Vertex objects.
     */
    std::unordered_map<uint64_t, Vertex> getVertices() const { return vertices_; }

    /**
     * Computes the shortest path using a modified bidirectional search algorithm. If standard is set to true, a standard bidirectional Dijkstra search is
     * conducted instead. The standard bidirectional Dijkstra search is only used for testing, as it is much slower than the modified bidirectional search.
     * @param source The ID of the source vertex.
     * @param target The ID of the target vertex.
     * @param standard If standard is set to true, a standard bidirectional Dijkstra search is conducted. The
     * standard bidirectional Dijkstra search is only used for testing, as it is much slower than the modified
     * bidirectional search. Otherwise, the modified bidirectional search is used.
     * @return A pair containing the shortest path (as OSM Node IDs) and the weight of the shortest path.
     */
    std::pair<std::vector<uint64_t>, double> getShortestPath(uint64_t source, uint64_t target, bool standard = false);

    /**
     * Converts a path that is in terms of OSM Node IDs to a path containing coordinates
     * @param path A path made up of OSM Node IDs.
     * @return A path that is made up of coordinates (i.e. arrays containing latitude and longitude).
     */
    std::vector<std::array<double, 2>> convertPathToCoordinates(const std::vector<uint64_t>& path);

    /**
     * Used to remove unnecessary edges after the graph is contracted. Edges that start at a Vertex of greater order than the Vertex the Edge ends at can
     * be removed from the graph because these edges will never appear on the shortest path.
    */
    void optimizeEdges();

    /**
     * Serializes necessary information so that the graph can be saved in a binary file. See cereal documentation.
     * @tparam Archive See cereal documentation.
     * @param ar See cereal documentation.
   */
    template <class Archive>
    void save(Archive& ar) const{ ar(vertices_, edges_, shortcuts_, locations_); }

    /**
     * Serializes necessary information so that the graph can be loaded from a binary file. See cereal documentation.
     * @tparam Archive See cereal documentation.
     * @param ar See cereal documentation.
   */
    template <class Archive>
    void load(Archive& ar) { ar(vertices_, edges_, shortcuts_, locations_); }
};

