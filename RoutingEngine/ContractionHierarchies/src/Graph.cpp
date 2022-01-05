#include "BidirectionalSearch.h"
#include "Graph.h"
#include <algorithm>
#include <cassert>
#include <utility>

Vertex::Vertex(const uint64_t id, const uint64_t order, const bool contracted) : id(id), order(order), deleted_neighbors(0) {}
Vertex::Vertex() : id(0), order(0), deleted_neighbors(0) {}

Edge::Edge(const uint64_t start, const uint64_t end, std::vector<uint64_t>& nodes, const double time_weight, const double distance_weight) : start(start), end(end), nodes(std::move(nodes)), time_weight(time_weight), distance_weight(distance_weight), weight(0) {}
Edge::Edge(const uint64_t start, const uint64_t end, const double weight) : start(start), end(end), time_weight(0), distance_weight(0), weight(weight) {}
Edge::Edge() : start(0), end(0), time_weight(0), distance_weight(0), weight(0) {}

Graph::Graph(std::unordered_map<uint64_t, std::array<double, 2>> locations) : locations_(std::move(locations)), num_edges_(0) {}
Graph::Graph() : num_edges_(0) {}

void Graph::addEdge(uint64_t start, uint64_t end, std::vector<uint64_t> *nodes, double time_weight, double distance_weight, bool bidirectional, bool time) {
    if (vertices_.find(start) == vertices_.end()) { vertices_.emplace(start, start); }
    if (vertices_.find(end) == vertices_.end()) { vertices_.emplace(end, end); }

    // The edge weight should never be negative.
    assert(time_weight >= 0);
    assert(distance_weight >= 0);

    edges_[start][end] = Edge(start, end, *nodes, time_weight, distance_weight);
    if (time) {
        vertices_[start].out_edges[end] = time_weight;
        vertices_[end].in_edges[start] = time_weight;
    }
    else {
        vertices_[start].out_edges[end] = distance_weight;
        vertices_[end].in_edges[start] = distance_weight;
    }
    num_edges_++;

    if (bidirectional) {
        std::reverse(nodes->begin(), nodes->end());
        edges_[end][start] = Edge(end, start, *nodes, time_weight, distance_weight);
        if (time) {
            vertices_[start].in_edges[end] = time_weight;
            vertices_[end].out_edges[start] = time_weight;
        }
        else {
            vertices_[start].in_edges[end] = distance_weight;
            vertices_[end].out_edges[start] = distance_weight;
        }
        num_edges_++;
    }
}

void Graph::addEdge(uint64_t start, uint64_t end, double weight, bool bidirectional) {
    // The edge weight should never be negative.
    assert(weight >= 0);
    if (vertices_.find(start) == vertices_.end()) { vertices_.emplace(start, start); }
    if (vertices_.find(end) == vertices_.end()) { vertices_.emplace(end, end); }

    edges_[start][end] = Edge(start, end, weight);
    vertices_[start].out_edges[end] = weight;
    vertices_[end].in_edges[start] = weight;
    num_edges_++;

    if (bidirectional) {
        edges_[end][start] = Edge(end, start, weight);
        vertices_[start].in_edges[end] = weight;
        vertices_[end].out_edges[start] = weight;
        num_edges_++;
    }
}

bool Graph::edgeExists(uint64_t start, uint64_t end) {
    if (vertices_.find(start) != vertices_.end() && vertices_[start].out_edges.find(end) != vertices_[start].out_edges.end()) {
        return true;
    }
    else {
        return false;
    }
}

void Graph::removeEdge(uint64_t start, uint64_t end) {
    if (edgeExists(start, end)) {
        vertices_[start].out_edges.erase(end);
        vertices_[end].in_edges.erase(start);
        num_edges_--;
    }
    else {
        std::ostringstream oss;
        oss << "Cannot remove edge from " << start << " to " << end << ". Edge does not exist.";
        std::string var = oss.str();
        throw std::logic_error(var);
    }
}
void Graph::addShortcut(uint64_t start, uint64_t end, uint64_t through, double weight) {
    // The edge weight should never be negative.
    assert(weight >= 0);
    shortcuts_[start][end] = through;
    vertices_[start].out_edges[end] = weight;
    vertices_[end].in_edges[start] = weight;
    num_edges_++;
}

void Graph::optimizeEdges() {
    // Loops through all edges in the graph and removes those that will never appear on the shortest path.
    for (const auto& vertex : vertices_) {
        auto it = vertices_[vertex.first].out_edges.begin();
        while (it != vertices_[vertex.first].out_edges.end()) {
            if (vertices_[vertex.first].order > vertices_[std::get<0>(*it)].order) {
                it = vertices_[vertex.first].out_edges.erase(it);
            }
            else {
                ++it;
            }
        }

        it = vertices_[vertex.first].in_edges.begin();

        while (it != vertices_[vertex.first].in_edges.end()) {
            if (vertices_[vertex.first].order > vertices_[std::get<0>(*it)].order) {
                it = vertices_[vertex.first].in_edges.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

void Graph::addOrdering(uint64_t vertex, uint64_t ordering) {
    // The ordering should never be negative.
    assert(ordering >= 0);
    if (vertices_.find(vertex) != vertices_.end()) { vertices_[vertex].order = ordering; }
}

std::pair<std::vector<uint64_t>, double> Graph::getShortestPath(uint64_t source, uint64_t target, bool standard) {
    if (vertices_.find(source) == vertices_.end() || vertices_.find(target) == vertices_.end()) {
        throw std::logic_error("Invalid vertex ID. Make sure that the source and target vertices exist.");
    }
    BidirectionalSearch searcher(&vertices_, &shortcuts_, &edges_);
    // If standard is set to true, then a standard bidirectional Dijkstra search is performed. The standard search is primarily used for testing.
    return searcher.executeSearch(source, target, standard);
}

std::vector<std::array<double, 2>> Graph::convertPathToCoordinates(const std::vector<uint64_t>& path) {
    std::vector<std::array<double, 2>> coordinates;
    coordinates.reserve(path.size());
    for (const auto& id : path) {
        coordinates.push_back(locations_.at(id));
    }
    return coordinates;
}

