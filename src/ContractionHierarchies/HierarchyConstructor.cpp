#include "HierarchyConstructor.h"

HierarchyConstructor::HierarchyConstructor(Graph& graph) : graph_(graph), total_edges_added_(0) { vertices_ = graph_.get_vertices(); }

void HierarchyConstructor::contract_graph() {
    // We construct the priority Queue by simulating the contraction of all vertices.
    Queue<HeapElement> queue = get_initial_ordering(1000);

    // The order in which the vertices are contracted must be recorded for route finding later on.
    uint64_t ordering_count = 0;

    std::cout << "Contracting graph..." << std::endl;

    while (!queue.empty()) {
        const auto contracted_vertex = get_next(1000, &queue);
        graph_.add_ordering(contracted_vertex, ordering_count);
        ordering_count++;
        if (ordering_count % 1000 == 0) { std::cout << "Contracted " << ordering_count << " vertices." << std::endl; }
        contract_vertex(contracted_vertex, 1000);

        // We update the deleted neighbors counter of all vertices adjacent to the Vertex being contracted.
        contracted_neighbors(contracted_vertex);

        /**
        * Removes all edges incident to the contracted Vertex, as they will not be useful in future contractions. This
        * prevents iterating over edges leading to vertices that have already been contracted.
        *
        * NOTE: It would seem that giving the Vertex a boolean attribute "contracted", indicating whether that Vertex has been
        * contracted or not, and simply ignoring those vertices that have "contracted" set to true during the contraction process
        * would be faster than removing the edges to a Vertex and deleting the Vertex. Testing has show that this is not the case.
        */
        for (const auto& v : vertices_[contracted_vertex].in_edges) {
            vertices_[v.first].out_edges.erase(contracted_vertex);
        }

        for (const auto& w : vertices_[contracted_vertex].out_edges) {
            vertices_[w.first].in_edges.erase(contracted_vertex);
        }

        vertices_.erase(contracted_vertex);
    }

    std::cout << "Added " << total_edges_added_ << " edges." << std::endl;
    std::cout << "Graph contraction phase complete." << std::endl;
    std::cout << "Optimizing edges..." << std::endl;

    // Optimizing the graph removes any edges that go from a Vertex of higher order to a Vertex of lower order, as these will never be on the shortest path.
    graph_.optimize_edges();
    std::cout << "Edge optimization phase complete." << std::endl;
}

int HierarchyConstructor::contract_vertex(const uint64_t contracted_vertex, const int hop_limit, const bool simulated) {
    std::vector<std::tuple<uint64_t, uint64_t, double>> shortcuts_to_add;
    shortcuts_to_add.reserve(5);
    int added_shortcuts = 0;
    const double max_out_distance = get_max_out_distance(contracted_vertex);

    // Loops through the incoming vertices of the contracted Vertex.
    for (const auto& v : vertices_[contracted_vertex].in_edges) {
        // We ignore the Vertex that is currently being contracted.
        if (v.first == contracted_vertex) { continue; }

        auto dists = witness_search(v.first, contracted_vertex, v.second + max_out_distance, hop_limit);

        // Loops through the outgoing vertices of the contracted Vertex.
        for (const auto& w : vertices_[contracted_vertex].out_edges) {
            // We ignore the Vertex that is currently being contracted.
            if (w.first == contracted_vertex || v.first == w.first) { continue; }

            // If no witness path was found, then we need to add a shortcut. Adding unnecessary shortcuts does not invalidate the algorithm.
            if (dists.find(w.first) == dists.end() || dists.at(w.first) > v.second + w.second) {
                if (vertices_[v.first].out_edges.find(w.first) == vertices_[v.first].out_edges.end() || vertices_[v.first].out_edges[w.first] > v.second + w.second) {
                    added_shortcuts++;
                    if (!simulated) { shortcuts_to_add.emplace_back(v.first, w.first, v.second + w.second); }
                }
            }
        }
    }
    if (!shortcuts_to_add.empty()) { add_shortcuts(contracted_vertex, &shortcuts_to_add); }
    return added_shortcuts;
}

std::unordered_map<uint64_t, double> HierarchyConstructor::witness_search(const uint64_t source, const uint64_t contracted_vertex, const double max_distance, const int hop_limit) {
    Queue<HeapElement> queue;
    std::unordered_map<uint64_t, double> dists;
    std::unordered_set<uint64_t> visited, targets;
    int hops = 0, targets_seen = 0;
    queue.push(HeapElement(source, 0));
    dists[source] = 0;

    for (const auto& w : vertices_[contracted_vertex].out_edges) {
        if (w.first != contracted_vertex) { targets.insert(w.first); }
    }

    // Standard Dijkstra search.
    while (!queue.empty() && targets_seen < targets.size() && queue.peek()->value <= max_distance && hops < hop_limit) {
        HeapElement u = queue.pop();
        hops++;
        visited.insert(u.id);

        if (targets.find(u.id) != targets.end()) { targets_seen++; }

        for (const auto& w : vertices_[u.id].out_edges) {
            if (visited.find(w.first) != visited.end() || w.first == contracted_vertex) { continue; }
            if (dists.find(w.first) == dists.end() || dists[w.first] > dists[u.id] + w.second) {
                dists[w.first] = dists[u.id] + w.second;
                queue.push(HeapElement(w.first, dists[w.first]));
            }
        }
    }
    return dists;
}

Queue<HeapElement> HierarchyConstructor::get_initial_ordering(const int hop_limit) {
    std::cout << "Getting initial node ordering..." << std::endl;
    Queue<HeapElement> queue(vertices_.size());

    // We simulate the contraction of all vertices in the graph to get a good node ordering.
    for (const auto& x : vertices_) {
        queue.push(HeapElement(x.first, get_priority_term(x.first, hop_limit, true)));
    }

    std::cout << "Node ordering phase complete." << std::endl;
    return queue;
}

uint64_t HierarchyConstructor::get_next(const int hop_limit, Queue<HeapElement>* queue) {
    uint64_t temp_vertex = 0;

    while (temp_vertex != queue->peek()->id) {
        temp_vertex = queue->peek()->id;
        // Lazy update.
        queue->lazyUpdate(HeapElement(queue->peek()->id, get_priority_term(temp_vertex, hop_limit)));
    }

    const uint64_t contracted_vertex = queue->pop().id;
    return contracted_vertex;
}

void HierarchyConstructor::add_shortcuts(const uint64_t contracted_vertex, const std::vector<std::tuple<uint64_t, uint64_t, double>>* shortcuts_needed) {
    for (const auto & it : *shortcuts_needed) {
        graph_.add_shortcut(std::get<0>(it), std::get<1>(it), contracted_vertex, std::get<2>(it));
        vertices_[std::get<0>(it)].out_edges[std::get<1>(it)] =  std::get<2>(it);
        vertices_[std::get<1>(it)].in_edges[std::get<0>(it)] = std::get<2>(it);
        total_edges_added_++;
    }
}

void HierarchyConstructor::contracted_neighbors(const uint64_t contracted_vertex) {

    // We use an unordered set to keep track of vertices that have already been incremented to avoid over counting.
    std::unordered_set<uint64_t> seen;

    // Updates the cost of all the incoming neighbors of the contracted Vertex.
    for (const auto& v : vertices_[contracted_vertex].in_edges) {
        if (seen.find(v.first) == seen.end() && v.first != contracted_vertex) {
            seen.insert(v.first);
            vertices_[v.first].deleted_neighbors++;
        }
    }

    // Updates the cost of all the outgoing neighbors of the contracted Vertex.
    for (const auto& w : vertices_[contracted_vertex].out_edges) {
        if (seen.find(w.first) == seen.end() && w.first != contracted_vertex) {
            seen.insert(w.first);
            vertices_[w.first].deleted_neighbors++;
        }
    }
}

double HierarchyConstructor::get_max_out_distance(const uint64_t contracted_vertex) const {
    double max_out = 0.0;

    for (const auto& w : vertices_.at(contracted_vertex).out_edges) {
        if ((w.second > max_out) && (w.first != contracted_vertex)) { max_out = w.second; }
    }

    return max_out;
}

int HierarchyConstructor::get_edge_difference(const uint64_t contracted_vertex, const int hop_limit) {
    // original_edges is the total number of incoming and outgoing edges that a Vertex has before contraction.
    int original_edges = vertices_[contracted_vertex].in_edges.size() + vertices_[contracted_vertex].out_edges.size();
    // added_shortcuts is the number of shortcuts that must be added after contraction of a Vertex.
    int added_shortcuts = contract_vertex(contracted_vertex, hop_limit, true);

    return 30 * added_shortcuts - original_edges;
}

int HierarchyConstructor::get_priority_term(const uint64_t contracted_vertex, const int hop_limit, const bool simulated) {
    if (simulated) {
        return get_edge_difference(contracted_vertex, hop_limit);
    }
    else {
        return get_edge_difference(contracted_vertex, hop_limit) + 20 * vertices_[contracted_vertex].deleted_neighbors;
    }
}
