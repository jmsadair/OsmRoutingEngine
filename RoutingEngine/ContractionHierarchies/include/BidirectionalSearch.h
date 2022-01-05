#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include "Queue.h"
#include "Graph.h"

/**
* The purpose of this class is to find the shortest path between two vertices in a graph. We implement two different
* search algorithms: the standard, bidirectional Dijkstra algorithm and a modified bidirectional search.
*/
class BidirectionalSearch {

private:

    // The best shortest path estimate is initially initialized to infinity, and improved throughout the search.
    const double INF_ = std::numeric_limits<double>::infinity();

    // The vertices that the search will be conducted on.
    const std::unordered_map<uint64_t, Vertex>* vertices_;

    // Used for unpacking the shortest path.
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>* shortcuts_;

    // The OSM node IDs that connect vertices in the graph.
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, Edge>>* edges_;

    // Keeps track of which vertices have been settled in the forward and reverse search.
    std::unordered_set<uint64_t> visited_source_, visited_target_, stalled_;

    // Keeps track of the length of the shortest path found so far for each Vertex encountered in the search.
    std::unordered_map<uint64_t, double> dist_source_, dist_target_;

    // Keeps track of the order in which the vertices are visited. Used for reconstructing the shortest path after
    // the search is terminated.
    std::unordered_map<uint64_t, uint64_t> prev_source_, prev_target_;

    /**
     * A standard, minimum, binary heap is used as the priority MinHeap for the search. Instead of using two priority
     * queues for the bidirectional search, we use one. Each item in the MinHeap stores the direction it represents.
     * A 1 indicates that a forward search should be performed when relaxing that that vertices edges and a 0 indicates
     * a backwards search.
     */
    Queue::MinHeap<HeapElement> queue_;

    /**
     * This method will be used during the the bidirectional search. The process of relaxing an Edge (u,v) consists of
     * testing whether we can improve the shortest path to v found so far by going through u and, if so, updating
     * dist_source[v] /dist_target[v] and prev_source[v] / prev_target[v]. A relaxation step may decrease the value of
     * the shortest path estimate dist_source[v] / dist_target[v] and update v's predecessors.
     * @param vertex_id The ID of the vertex currently being settled.
     * @param backward Indicates whether we are performing a backward search or a forward search. True if backward,
     * false otherwise.
     * @param standard If standard is set to true, then we perform a bidirectional Dijkstra search. Otherwise,
     * we perform a modified search.
     */
    void relax_edge(uint64_t vertex_id, bool backward = false, bool standard = false);

    /**
     * Retrieves the appropriate set of edges for the given search (i.e. if we are relaxing edges during the forward
     * search, then this method will return the outgoing edges of the vertex being settled).
     * @param vertex_id The ID of the vertex that we are getting the edges for.
     * @param backward Indicates whether we are performing a backwards search or a forward search. If backwards, then
     * we return the incoming edges of the vertex. Otherwise, we return the outgoing edges.
     * @return A hashmap that maps vertex IDs to weights.
     */
    const std::unordered_map<uint64_t, double>& getAllowedEdges(uint64_t vertex_id, bool backward);

    /**
     * Retrieves the appropriate distance estimates for the given search.
     * @param backward  Indicates whether we are performing a backwards search or a forward search. If backwards, then
     * the distance estimates for the backward search are returned. Otherwise, the distance estimates for the forward
     * search are returned.
     * @return The best distance estimates for the search.
     */
    std::unordered_map<uint64_t, double>& getAllowedDists(bool backward);

    /**
     * Retrieves the appropriate visited set for for the given search.
     * @param backward Indicates whether we are performing a backwards search or a forwards search. If backwards, then
     * the visited set for the backwards search is returned. Otherwise, the visited set for the forward search is
     * returned.
     * @return An unordered set containing the vertices that have been settled on the search.
     */
    std::unordered_set<uint64_t>& getAllowedVisited(bool backward);

    /**
     * Retrieves the appropriate parent pointers for the given search. Primarily used for path reconstruction.
     * @param backward Indicates whether we are performing a backwards search or a forwards search. If backwards, returns
     * the parent pointers that corresponds the backward search. Otherwise, returns the parent pointers that correspond
     * to the forward search.
     * @return A hashmap that maps vertex IDs to the the their parent vertex ID.
     */
    std::unordered_map<uint64_t, uint64_t>& getAllowedPrev(bool backward);

    /**
     * The purpose of this method is to reconstruct the shortest path that determined by the bidirectional search.
     * @param source The ID of the source vertex.
     * @param target The ID of the target vertex.
     * @param intersection The ID of the vertex at which the forward and backward searches meet (note that this might not be the first vertex at
     * which the searches meet).
     * @return A vector of vertex IDs that make up the shortest path (the shortcuts must still be unpacked and the edges must still be inserted).
     */
    std::vector<uint64_t> reconstructPath(uint64_t source, uint64_t target, uint64_t intersection);

    /**
     * Given two vertices, unpacks all of the shortcuts that connect those vertices.
     * @param start The ID of the first vertex.
     * @param end The ID of the second vertex.
     * @return A vector containing all of the vertex IDs that shortcut edges go through.
     */
    std::vector<uint64_t> unpackShortcut(uint64_t start, uint64_t end);

    /**
     * Unpacks all the shortcuts in a reconstructed path and outputs a complete path.
     * @param path A path that has been reconstructed after the search process is complete.
     * @return A vector representing the shortest path between the source and target vertices in the graph. Note that
     * this path does not contain the vertex IDs that are contained in the graph edges.
     */
    std::vector<uint64_t> unpackPath(std::vector<uint64_t>* path);

    /**
     * Inserts the vertex IDs that are connect two adjacent vertices in the shortest path into the path. More
     * specifically, the vertices in the search graph represent road network intersections; we must insert the OSM Node
     * IDs that connect those intersections in order for the path to be complete.
     * @param path A path that has has been completely unpacked.
     * @return A complete path that is ready to be used for routing.
     */
    std::vector<uint64_t> insertEdgeNodes(const std::vector<uint64_t>& path);

    /**
     * Indicates whether a shortcut edge connects two vertices.
     * @param start The first vertex ID.
     * @param end The second vertex ID.
     * @return A boolean indicating whether a shortcut edge connects the given vertices.
     */
    bool isShortcut(uint64_t start, uint64_t end);


public:

    /**
     * The constructor for the BidirectionalSearch class.
     * @param vertices The vertices that the search will be conducted on.
     * @param shortcuts The shortcut edges present in the graph, if any.
     * @param edges The edge data for the graph.
     */
    BidirectionalSearch(const std::unordered_map<uint64_t, Vertex>* vertices,
                        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>* shortcuts,
                        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, Edge>>* edges);

    /**
     * This is the primary search we will use for routing. Provides the option of running a bidirectional Dijkstra search
     * or a modified bidirectional search. The modified bidirectional search is similar to a bidirectional Dijkstra search,
     * but instead of relaxing all edges that are adjacent to a Vertex that is popped from the MinHeap, we only relax
     * edges that lead to nodes with higher order than that Vertex. In turn, this search is much faster than a
     * bidirectional Dijkstra search.
     * @param source The ID of the source vertex.
     * @param target The ID of the target vertex,
     * @param standard boolean value standard indicates which search is performed. If standard is set to true, then a
     * bidirectional Dijkstra search will be ran. Otherwise, the modified bidirectional search is ran.
     * @return A pair containing the shortest path and the length of the shortest path.
     */
    std::pair<std::vector<uint64_t>, double> executeSearch(uint64_t source, uint64_t target, bool standard);

};