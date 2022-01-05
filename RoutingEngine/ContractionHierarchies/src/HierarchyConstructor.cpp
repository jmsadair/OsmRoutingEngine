#include "HierarchyConstructor.h"

HierarchyConstructor::HierarchyConstructor(Graph& graph, const int edge_difference_coefficient, const int deleted_neigbhors_coefficient)
    : graph_(graph), total_edges_added_(0), edge_difference_coefficient(edge_difference_coefficient), deleted_neighbors_coefficient(deleted_neigbhors_coefficient) { vertices_ = graph_.getVertices(); }

void HierarchyConstructor::contractGraph() {
    // We construct the priority MinHeap by simulating the contraction of all vertices.
    Queue::MinHeap<HeapElement> queue = getInitialOrdering();
    // The order in which the vertices are contracted must be recorded for route finding later on.
    uint64_t ordering_count = 0;

    while (!queue.empty()) {
        const auto contracted_vertex = getNext(&queue);
        graph_.addOrdering(contracted_vertex, ordering_count);
        ordering_count++;
        contractVertex(contracted_vertex);

        // We update the deleted neighbors counter of all vertices adjacent to the Vertex being contracted.
        contractedNeighbors(contracted_vertex);

        /**
        * Removes all edges incident to the contracted Vertex, as they will not be useful in future contractions. This
        * prevents iterating over edges leading to vertices that have already been contracted.
        *
        * NOTE: It would seem that giving the Vertex a boolean attribute "contracted", indicating whether that Vertex has been
        * contracted or not, and simply ignoring those vertices that have "contracted" set to true during the contraction process
        * would be faster than removing the edges to a Vertex and deleting the Vertex. Testing has show that this is not the case.
        */
        for (const auto& [incoming_id, incoming_weight] : vertices_[contracted_vertex].in_edges) {
            vertices_[incoming_id].out_edges.erase(contracted_vertex);
        }

        for (const auto& [outgoing_id, outgoing_weight] : vertices_[contracted_vertex].out_edges) {
            vertices_[outgoing_id].in_edges.erase(contracted_vertex);
        }

        vertices_.erase(contracted_vertex);
    }

    // Optimizing the graph removes any edges that go from a Vertex of higher order to a Vertex of lower order, as these will never be on the shortest path.
    graph_.optimizeEdges();
}

int HierarchyConstructor::contractVertex(uint64_t contracted_vertex, bool simulated) {
    std::vector<std::tuple<uint64_t, uint64_t, double>> shortcuts_to_add;
    shortcuts_to_add.reserve(5);
    int added_shortcuts = 0;
    const double max_out_distance = getMaxOutDistance(contracted_vertex);

    // Loops through the incoming vertices of the contracted Vertex.
    for (const auto& [incoming_id, incoming_weight] : vertices_[contracted_vertex].in_edges) {
        // We ignore the Vertex that is currently being contracted.
        if (incoming_id == contracted_vertex) { continue; }

        auto dists = witnessSearch(incoming_id, contracted_vertex, incoming_weight + max_out_distance);

        // Loops through the outgoing vertices of the contracted Vertex.
        for (const auto& [outgoing_id, outgoing_weight] : vertices_[contracted_vertex].out_edges) {
            // We ignore the Vertex that is currently being contracted.
            if (outgoing_id == contracted_vertex || incoming_id == outgoing_id) { continue; }

            // If no witness path was found, then we need to add a shortcut. Adding unnecessary shortcuts does not invalidate the algorithm.
            if (dists.find(outgoing_id) == dists.end() || dists.at(outgoing_id) > incoming_weight + outgoing_weight) {
                if (vertices_[incoming_id].out_edges.find(outgoing_id) == vertices_[incoming_id].out_edges.end() || vertices_[incoming_id].out_edges[outgoing_id] > incoming_weight + outgoing_weight) {
                    added_shortcuts++;
                    if (!simulated) { shortcuts_to_add.emplace_back(incoming_id, outgoing_id, incoming_weight + outgoing_weight); }
                }
            }
        }
    }
    if (!shortcuts_to_add.empty()) { addShortcuts(contracted_vertex, &shortcuts_to_add); }
    return added_shortcuts;
}

std::unordered_map<uint64_t, double>
HierarchyConstructor::witnessSearch(uint64_t source, uint64_t contracted_vertex, double max_distance) {
    Queue::MinHeap<HeapElement> queue;
    std::unordered_map<uint64_t, double> dists;
    std::unordered_set<uint64_t> visited, targets;
    int hops = 0, targets_seen = 0;
    queue.push(HeapElement(source, 0));
    dists[source] = 0;

    for (const auto& [outgoing_id, outgoing_weight] : vertices_[contracted_vertex].out_edges) {
        if (outgoing_id != contracted_vertex) { targets.insert(outgoing_id); }
    }

    // Standard Dijkstra search.
    while (!queue.empty() && targets_seen < targets.size() && queue.peek().value <= max_distance && hops < HOP_LIMIT) {
        HeapElement u = queue.pop();
        hops++;
        visited.insert(u.id);

        if (targets.find(u.id) != targets.end()) { targets_seen++; }

        for (const auto& [outgoing_id, outgoing_weight] : vertices_[u.id].out_edges) {
            if (visited.find(outgoing_id) != visited.end() || outgoing_id == contracted_vertex) { continue; }
            if (dists.find(outgoing_id) == dists.end() || dists[outgoing_id] > dists[u.id] + outgoing_weight) {
                dists[outgoing_id] = dists[u.id] + outgoing_weight;
                queue.push(HeapElement(outgoing_id, dists[outgoing_id]));
            }
        }
    }
    return dists;
}

Queue::MinHeap<HeapElement> HierarchyConstructor::getInitialOrdering() {
    Queue::MinHeap<HeapElement> queue(int(vertices_.size()));
    // We simulate the contraction of all vertices in the graph to get a good node ordering.
    for (const auto& [id, vertex] : vertices_) {
        queue.push(HeapElement(id, getPriorityTerm(id, true)));
    }
    return queue;
}

uint64_t HierarchyConstructor::getNext(Queue::MinHeap<HeapElement> *queue) {
    uint64_t temp_vertex = 0;
    while (temp_vertex != queue->peek().id) {
        temp_vertex = queue->peek().id;
        // Lazy update.
        queue->lazyUpdate(HeapElement(queue->peek().id, getPriorityTerm(queue->peek().id)));
    }
    const uint64_t contracted_vertex = queue->pop().id;
    return contracted_vertex;
}

void HierarchyConstructor::addShortcuts(uint64_t contracted_vertex, const std::vector<std::tuple<uint64_t, uint64_t, double>>* shortcuts) {
    for (const auto & it : *shortcuts) {
        graph_.addShortcut(std::get<0>(it), std::get<1>(it), contracted_vertex, std::get<2>(it));
        vertices_[std::get<0>(it)].out_edges[std::get<1>(it)] =  std::get<2>(it);
        vertices_[std::get<1>(it)].in_edges[std::get<0>(it)] = std::get<2>(it);
        total_edges_added_++;
    }
}

void HierarchyConstructor::contractedNeighbors(uint64_t contracted_vertex) {
    // We use an unordered set to keep track of vertices that have already been incremented to avoid over counting.
    std::unordered_set<uint64_t> seen;

    // Updates the cost of all the incoming neighbors of the contracted Vertex.
    for (const auto& [incoming_id, incoming_weight] : vertices_[contracted_vertex].in_edges) {
        if (seen.find(incoming_id) == seen.end() && incoming_id != contracted_vertex) {
            seen.insert(incoming_id);
            vertices_[incoming_id].deleted_neighbors++;
        }
    }

    // Updates the cost of all the outgoing neighbors of the contracted Vertex.
    for (const auto& [outgoing_id, outgoing_weight] : vertices_[contracted_vertex].out_edges) {
        if (seen.find(outgoing_id) == seen.end() && outgoing_id != contracted_vertex) {
            seen.insert(outgoing_id);
            vertices_[outgoing_id].deleted_neighbors++;
        }
    }
}

double HierarchyConstructor::getMaxOutDistance(uint64_t contracted_vertex) const {
    double max_out = 0.0;
    for (const auto& [outgoing_id, outgoing_weight] : vertices_.at(contracted_vertex).out_edges) {
        if ((outgoing_weight > max_out) && (outgoing_id != contracted_vertex)) { max_out = outgoing_weight; }
    }
    return max_out;
}

int HierarchyConstructor::getEdgeDifference(uint64_t contracted_vertex) {
    // original_edges is the total number of incoming and outgoing edges that a Vertex has before contraction.
    uint64_t original_edges = vertices_[contracted_vertex].in_edges.size() + vertices_[contracted_vertex].out_edges.size();
    // added_shortcuts is the number of shortcuts that must be added after contraction of a Vertex.
    int added_shortcuts = contractVertex(contracted_vertex,true);

    return int(added_shortcuts - original_edges);
}

int HierarchyConstructor::getPriorityTerm(uint64_t contracted_vertex, bool simulated) {
    if (simulated) {
        return getEdgeDifference(contracted_vertex);
    }
    else {
        return edge_difference_coefficient * getEdgeDifference(contracted_vertex) + deleted_neighbors_coefficient * vertices_[contracted_vertex].deleted_neighbors;
    }
}

