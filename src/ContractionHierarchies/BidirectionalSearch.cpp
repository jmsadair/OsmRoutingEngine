#include "BidirectionalSearch.h"
#include <algorithm>

BidirectionalSearch::BidirectionalSearch(const std::unordered_map<uint64_t, Vertex>*vertices, const std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>*shortcuts)
        : vertices_(vertices), shortcuts_(shortcuts), queue_(100)
{}

std::pair<std::vector<uint64_t>, double> BidirectionalSearch::executeSearch(uint64_t source, uint64_t target, bool standard) {
    // u is the Vertex being settled and intersection is the Vertex at which the forward and backwards search meet.
    uint64_t u, intersection = 0;
    // length of the shortest path found so far.
    double best = INF_;
    dist_source_[source] = 0;
    dist_target_[target] = 0;
    prev_source_[source] = -1;
    prev_target_[target] = -1;
    queue_.push(HeapElement(source, 0, 1));
    queue_.push(HeapElement(target, 0, 0));

    while (!queue_.empty()) {
        if (queue_.peek().direction == 1) {
            u = queue_.pop().id;
            visited_source_.insert(u);
            relax_edge(u, false, standard);
        }
        else {
            u = queue_.pop().id;
            visited_target_.insert(u);
            relax_edge(u, true, standard);
        }
        /**
        * It is not sufficient to abort the search as soon as the backward search and forward search meet.
        * We instead abort the search when the length of the shortest path found so far is less than or
        * equal to the distance to the next Vertex in the Queue.
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
        if (standard) {
            return std::make_pair(path, dist_source_[intersection] + dist_target_[intersection]);
        } else {
            return std::make_pair(unpackPath(&path), dist_source_[intersection] + dist_target_[intersection]);
        }
    }
    else {
        return std::make_pair(std::vector<uint64_t>{}, -1);
    }
}

void BidirectionalSearch::relax_edge(const uint64_t vertex_id, const bool backward, const bool standard) {
    // If standard is set to true, then a standard, bidirectional Dijkstra search is being performed and the relaxation process is different.
    if (!standard) {
        // Relaxes the outgoing edges to the Vertex.
        if (!backward) {
            for (const auto& w : vertices_->at(vertex_id).out_edges) {
                // We only relax edges leading to a higher priority Vertex.
                if (visited_source_.find(w.first) != visited_source_.end() || vertices_->at(w.first).order < vertices_->at(vertex_id).order) {
                    continue;
                }
                if (dist_source_.find(w.first) == dist_source_.end() || dist_source_[w.first] > dist_source_[vertex_id] + w.second) {
                    dist_source_[w.first] = dist_source_[vertex_id] + w.second;
                    queue_.push(HeapElement(w.first, dist_source_[w.first], 1));
                    prev_source_[w.first] = vertex_id;
                }
            }
        }
        else {
            // Relaxes the incoming edges to the Vertex.
            for (const auto& v : vertices_->at(vertex_id).in_edges) {
                // We only relax edges leading to a higher priority Vertex.
                if (visited_target_.find(v.first) != visited_target_.end() || vertices_->at(v.first).order < vertices_->at(vertex_id).order) {
                    continue;
                }
                if (dist_target_.find(v.first) == dist_target_.end() || dist_target_[v.first] > dist_target_[vertex_id] + v.second) {
                    dist_target_[v.first] = dist_target_[vertex_id] + v.second;
                    queue_.push(HeapElement(v.first, dist_target_[v.first], 0));
                    prev_target_[v.first] = vertex_id;
                }
            }
        }
    }
    else {
        if (!backward) {
            // Relaxes the outgoing edges to the Vertex.
            for (const auto& w : vertices_->at(vertex_id).out_edges) {
                if (visited_source_.find(w.first) != visited_source_.end()) { continue; }
                if (dist_source_.find(w.first) == dist_source_.end() || dist_source_[w.first] > dist_source_[vertex_id] + w.second) {
                    dist_source_[w.first] = dist_source_[vertex_id] + w.second;
                    queue_.push(HeapElement(w.first, dist_source_[w.first], 1));
                    prev_source_[w.first] = vertex_id;
                }
            }
        }
        else {
            // Relaxes the incoming edges to the Vertex.
            for (const auto& v : vertices_->at(vertex_id).in_edges) {
                if (visited_target_.find(v.first) != visited_target_.end()) { continue; }
                if (dist_target_.find(v.first) == dist_target_.end() || dist_target_[v.first] > dist_target_[vertex_id] + v.second) {
                    dist_target_[v.first] = dist_target_[vertex_id] + v.second;
                    queue_.push(HeapElement(v.first, dist_target_[v.first], 0));
                    prev_target_[v.first] = vertex_id;
                }
            }
        }
    }
}

std::vector<uint64_t> BidirectionalSearch::reconstructPath(uint64_t source, uint64_t target, uint64_t intersection) {
    std::vector<uint64_t> path, path_source, path_target;
    path_source.reserve(prev_source_.size());
    path_target.reserve(prev_target_.size());
    path.reserve(prev_source_.size() + prev_target_.size());

    auto vertex_one_id = prev_source_[intersection];
    auto vertex_two_id = intersection;
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

bool BidirectionalSearch::isShortcut(uint64_t start, uint64_t end) {
    if (shortcuts_->find(start) != shortcuts_->end() && shortcuts_->at(start).find(end) != shortcuts_->at(start).end()) {
        return true;
    }
    return false;
}