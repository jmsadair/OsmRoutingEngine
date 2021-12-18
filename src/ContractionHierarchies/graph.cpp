#include "bidirectionalSearch.h"
#include "graph.h"
#include <algorithm>

vertex::vertex(const uint64_t id, const uint64_t order, const bool contracted) : id(id), order(order), deleted_neighbors(0) {}

vertex::vertex() :id(0), order(0), deleted_neighbors(0) {}

edge::edge(const uint64_t start, const uint64_t end, std::vector<uint64_t>& nodes, double weight) : start(start), end(end), nodes(nodes), weight(weight) {}

edge::edge() : start(0), end(0), weight(0) {}

graph::graph() :num_edges_(0) {}

void graph::add_edge(const uint64_t start, const uint64_t end, std::vector<uint64_t>& nodes, const double weight, const bool bidirectional) {
    if (vertices_.find(start) == vertices_.end()) { vertices_.emplace(start, start); }
    if (vertices_.find(end) == vertices_.end()) { vertices_.emplace(end, end); }

    edges_[start][end] = edge(start, end, nodes, weight);
    vertices_[start].out_edges[end] = weight;
    vertices_[end].in_edges[start] = weight;
    num_edges_++;

    if (bidirectional) {
        std::reverse(nodes.begin(), nodes.end());
        edges_[end][start] = edge(end, start, nodes, weight);
        vertices_[start].in_edges[end] = weight;
        vertices_[end].out_edges[start] = weight;
        num_edges_++;
    }
}

bool graph::edge_exists(const uint64_t start, const uint64_t end) {
    if (vertices_.find(start) != vertices_.end() && vertices_[start].out_edges.find(end) != vertices_[start].out_edges.end()) {
        return true;
    }
    else {
        return false;
    }
}

void graph::remove_edge(const uint64_t start, const uint64_t end) {
    if (edge_exists(start, end)) {
        vertices_[start].out_edges.erase(end);
        vertices_[end].in_edges.erase(start);
    }
}
void graph::add_shortcut(const uint64_t start, const uint64_t end, const uint64_t through, const double weight) {
    shortcuts_[start][end] = through;
    vertices_[start].out_edges[end] = weight;
    vertices_[end].in_edges[start] = weight;
    num_edges_++;
}

void graph::optimize_edges() {
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

void graph::add_ordering(const uint64_t vertex, const uint64_t ordering) {
    if (vertices_.find(vertex) != vertices_.end()) { vertices_[vertex].order = ordering; }
}

std::pair<std::vector<uint64_t>, double> graph::get_shortest_path(const uint64_t source, const uint64_t target, const bool standard) {
    bidirectional_search searcher(&vertices_, &shortcuts_);
    // If standard is set to true, then a standard bidirectional Dijkstra search is performed. The standard search is primarily used for testing.
    if (standard) {
        return searcher.execute_standard_search(source, target);
    }
    else {
        return searcher.execute_search(source, target);
    }
}
