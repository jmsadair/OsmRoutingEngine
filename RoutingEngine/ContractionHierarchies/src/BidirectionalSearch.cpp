#include "BidirectionalSearch.h"
#include <algorithm>
#include <cassert>

BidirectionalSearch::BidirectionalSearch(const std::unordered_map<uint64_t, Vertex>*vertices,
                                         const std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>*shortcuts,
                                         const std::unordered_map<uint64_t, std::unordered_map<uint64_t, Edge>>*edges)
        : vertices_(vertices), shortcuts_(shortcuts), edges_(edges), queue_(100)
{}

const std::unordered_map<uint64_t, double>& BidirectionalSearch::getAllowedEdges(const uint64_t vertex_id, const bool backward) {
    if (backward) {
        const auto& edges = vertices_->at(vertex_id).in_edges;
        return edges;
    }
    const auto& edges = vertices_->at(vertex_id).out_edges;
    return edges;
}

std::unordered_map<uint64_t, double>& BidirectionalSearch::getAllowedDists(const bool backward) {
    if (backward) {
        auto& dist = dist_target_;
        return dist;
    }
    auto& dist = dist_source_;
    return dist;
}

std::unordered_set<uint64_t>& BidirectionalSearch::getAllowedVisited(bool backward) {
    if (backward) {
        auto& visited = visited_target_;
        return visited;
    }
    auto& visited = visited_source_;
    return visited;
}

std::unordered_map<uint64_t, uint64_t>& BidirectionalSearch::getAllowedPrev(bool backward) {
    if (backward) {
        auto& prev = prev_target_;
        return prev;
    }
    auto& prev = prev_source_;
    return prev;
}

std::pair<std::vector<uint64_t>, double> BidirectionalSearch::executeSearch(uint64_t source, uint64_t target, bool standard) {
    // u is the Vertex being settled and intersection is the Vertex at which the forward and backwards search meet.
    uint64_t u  = 0;
    uint64_t intersection = 0;
    // length of the shortest path found so far.
    double best = INF_;
    dist_source_[source] = 0;
    dist_target_[target] = 0;
    prev_source_[source] = -1;
    prev_target_[target] = -1;
    queue_.push(HeapElement(source, 0, 1));
    queue_.push(HeapElement(target, 0, 0));

    while (!queue_.empty()) {
        u = queue_.peek().id;
        relax_edge(u, !bool(queue_.peek().direction), standard);
        /**
        * It is not sufficient to abort the search as soon as the backward search and forward search meet.
        * We instead abort the search when the length of the shortest path found so far is less than or
        * equal to the distance to the next Vertex in the MinHeap.
        */
        if (visited_source_.find(u) != visited_source_.end() && visited_target_.find(u) != visited_target_.end()
            && dist_source_[u] + dist_target_[u] < best) {
            intersection = u;
            best = dist_source_[u] + dist_target_[u];
            if (queue_.empty() || best <= queue_.peek().value) { break; }
        }
    }

    if (intersection != 0) {
        auto path = reconstructPath(source, target, intersection);
        // Path should never have a negative distance.
        assert(dist_source_[intersection] + dist_target_[intersection] >= 0);
        if (standard) {
            return std::make_pair(insertEdgeNodes(path), dist_source_[intersection] + dist_target_[intersection]);
        } else {
            return std::make_pair(insertEdgeNodes(unpackPath(&path)), dist_source_[intersection] + dist_target_[intersection]);
        }
    }
    else {
        return std::make_pair(std::vector<uint64_t>{}, -1);
    }
}

void BidirectionalSearch::relax_edge(const uint64_t vertex_id, const bool backward, const bool standard) {
    // Gets the relevant edges, distances, parent pointers, and visited data depending on whether it is a forward or backward search.
    auto& edges = getAllowedEdges(vertex_id, backward);
    auto& dist = getAllowedDists(backward);
    auto& prev = getAllowedPrev(backward);
    auto& visited = getAllowedVisited(backward);
    visited.insert(vertex_id);
    queue_.pop();

    // If standard is set to true, then a standard, bidirectional Dijkstra search is being performed and the relaxation process is different.
    if (!standard) {
        // Relaxes the incoming/outgoing edges to the vertex depending on whether it is a forward or backward search.
        for (const auto&[id, weight]: edges) {
            // We only relax edges leading to a higher priority Vertex.
            if (visited.find(id) != visited.end() || vertices_->at(id).order < vertices_->at(vertex_id).order) {
                continue;
            }
            // A new best distance estimate has been found.
            if (dist.find(id) == dist.end() || dist[id] > dist[vertex_id] + weight) {
                dist[id] = dist[vertex_id] + weight;
                queue_.push(HeapElement(id, dist[id], int(!backward)));
                prev[id] = vertex_id;
            }
        }
    }
    else {
        // Relaxes the incoming/outgoing edges to the vertex depending on whether it is a forward or backward search.
        for (const auto&[id, weight]: edges) {
            // We only relax edges leading to a higher priority Vertex.
            if (visited.find(id) != visited.end()) {
                continue;
            }
            // A new best distance estimate has been found.
            if (dist.find(id) == dist.end() || dist[id] > dist[vertex_id] + weight) {
                dist[id] = dist[vertex_id] + weight;
                queue_.push(HeapElement(id, dist[id], int(!backward)));
                prev[id] = vertex_id;
            }
        }
    }
}

std::vector<uint64_t> BidirectionalSearch::reconstructPath(uint64_t source, uint64_t target, uint64_t intersection) {
    std::vector<uint64_t> path, path_source, path_target;
    path_source.reserve(prev_source_.size());
    path_target.reserve(prev_target_.size());
    path.reserve(prev_source_.size() + prev_target_.size());

    uint64_t vertex_one_id = prev_source_[intersection];
    uint64_t vertex_two_id = intersection;
    while (vertex_one_id != -1 || vertex_two_id != -1) {
        if (vertex_one_id != -1) {
            path_source.push_back(vertex_one_id);
            vertex_one_id = prev_source_[vertex_one_id];
        }
        if (vertex_two_id != -1) {
            path_target.push_back(vertex_two_id);
            vertex_two_id = prev_target_[vertex_two_id];
        }
    }
    std::reverse(path_source.begin(), path_source.end());
    path.reserve(path_source.size() + path_target.size());
    path.insert(path.end(), path_source.begin(), path_source.end());
    path.insert(path.end(), path_target.begin(), path_target.end());
    return path;
}

std::vector<uint64_t> BidirectionalSearch::unpackShortcut(uint64_t start, uint64_t end) {
    std::vector<uint64_t> stack;
    std::vector<uint64_t> unpacked_shortcuts;
    stack.push_back(end);

    while (!stack.empty()) {
        auto next_id = stack.back();
        if (isShortcut(start, next_id)) {
            stack.push_back(shortcuts_->at(start).at(next_id));
        }
        else {
            unpacked_shortcuts.push_back(next_id);
            stack.pop_back();
            start = next_id;
        }
    }
    return unpacked_shortcuts;
}

std::vector<uint64_t> BidirectionalSearch::unpackPath(std::vector<uint64_t>* shortest_path) {
    int i = 0;
    std::vector<uint64_t> path;
    path.push_back(shortest_path->at(0));

    // Unpacks all shortcuts in the path.
    while (i < shortest_path->size() - 1) {
        if (isShortcut(shortest_path->at(i), shortest_path->at(i + 1))) {
            auto unpacked_shortcuts = unpackShortcut(shortest_path->at(i), shortest_path->at(i + 1));
            path.insert(path.end(), unpacked_shortcuts.begin(), unpacked_shortcuts.end());
        }
        else {
            path.push_back(shortest_path->at(i + 1));
        }
        i++;
    }

    return path;
}

std::vector<uint64_t> BidirectionalSearch::insertEdgeNodes(const std::vector<uint64_t>& path) {
    std::vector<uint64_t> complete_path{path[0]};
    int i = 0;
    while (i + 1 < path.size()) {
        complete_path.insert(complete_path.end(), edges_->at(path[i]).at(path[i + 1]).nodes.begin(), edges_->at(path[i]).at(path[i + 1]).nodes.end());
        complete_path.push_back(edges_->at(path[i]).at(path[i + 1]).end);
        i++;
    }
    return complete_path;
}

bool BidirectionalSearch::isShortcut(uint64_t start, uint64_t end) {
    if (shortcuts_->find(start) != shortcuts_->end() && shortcuts_->at(start).find(end) != shortcuts_->at(start).end()) {
        return true;
    }
    return false;
}