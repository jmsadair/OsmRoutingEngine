#include "bidirectionalSearch.h"

bidirectional_search::bidirectional_search(const std::unordered_map<unsigned long long, vertex>*vertices, const std::unordered_map<unsigned long long, std::unordered_map<unsigned long long, unsigned long long>>*shortcuts)
	: vertices_(vertices), shortcuts_(shortcuts), queue_(100)
{}

std::pair<std::vector<unsigned long long>, double> bidirectional_search::execute_search(const unsigned long long source, const unsigned long long target) {

	// u is the vertex being settled and intersection is the vertex at which the forward and backwards search meet. 
	unsigned long long u, intersection = 0;

	// length of the shortest path found so far.
	double best = INF_;

	dist_source_[source] = 0.0;
	dist_target_[target] = 0.0;

	prev_source_[source] = -1;
	prev_target_[target] = -1;

	queue_.push(source, 0.0, 1);
	queue_.push(target, 0.0, 0);

	while (!queue_.is_empty()) {
		
		if (queue_.peek_direction() == 1) {

			u = queue_.pop();
			visited_source_.insert(u);
			relax_edge(u, false, false);
		}
		else {

			u = queue_.pop();
			visited_target_.insert(u);
			relax_edge(u, true, false);
		}

		/**
		* It is not sufficient to abort the search as soon as the backward search and forward seach meet. 
		* We instead abort the search when the length of the shortest path found so far is less than or 
		* equal to the distance to the next vertex in the queue.
		*/
		if (visited_source_.find(u) != visited_source_.end() && visited_target_.find(u) != visited_target_.end() 
			&& dist_source_[u] + dist_target_[u] < best) {

			intersection = u;
			best = dist_source_[u] + dist_target_[u];

			if (best <= queue_.peek_value()) {

				break;
			}
		}
	}

	auto shortest_path = reconstruct_path(source, target, intersection);
	return std::make_pair(unpack_path(&shortest_path), dist_source_[intersection] + dist_target_[intersection]);
}

std::pair<std::vector<unsigned long long>, double> bidirectional_search::execute_standard_search(const unsigned long long source, const unsigned long long target) {

	// u is the vertex being settled and intersection is the vertex at which the forward and backwards search meet. 
	unsigned long long u, intersection = 0;

	// length of the shortest path found so far.
	double best = INF_;

	dist_source_[source] = 0.0;
	dist_target_[target] = 0.0;

	prev_source_[source] = -1;
	prev_target_[target] = -1;

	queue_.push(source, 0.0, 1);
	queue_.push(target, 0.0, 0);

	while (!queue_.is_empty()) {
		
		if (queue_.peek_direction() == 1) {

			u = queue_.pop();
			visited_source_.insert(u);
			relax_edge(u, false, true);
		}
		else {

			u = queue_.pop();
			visited_target_.insert(u);
			relax_edge(u, true, true);
		}

		/**
		* It is not sufficient to abort the search as soon as the backward search and forward seach meet.
		* We instead abort the search when the length of the shortest path found so far is less than or
		* equal to the distance to the next vertex in the queue.
		*/
		if (visited_source_.find(u) != visited_source_.end() && visited_target_.find(u) != visited_target_.end() && dist_source_[u] + dist_target_[u] < best) {

			intersection = u;
			best = dist_source_[u] + dist_target_[u];
			
			if (best <= queue_.peek_value()) {

				break;
			}
		}
	}
	
	auto path = reconstruct_path(source, target, intersection);
	return std::make_pair(path, dist_source_[intersection] + dist_target_[intersection]);
}

void bidirectional_search::relax_edge(const unsigned long long vertex_id, const bool backward, const bool standard) {

	// If standard is set to true, then a standard, bidirectional Dijkstra search is being performed and the relaxtion process is different.
	if (!standard) {

		// Relaxes the outgoing egdes to the vertex.
		if (!backward) {

			for (const auto& w : vertices_->at(vertex_id).out_edges) {
				
				// We only relax edges leading to a higher priority vertex.
				if (visited_source_.find(w.first) != visited_source_.end() || vertices_->at(w.first).order < vertices_->at(vertex_id).order) {

					continue;
				}

				if (dist_source_.find(w.first) == dist_source_.end() || dist_source_[w.first] > dist_source_[vertex_id] + w.second) {

					dist_source_[w.first] = dist_source_[vertex_id] + w.second;
					queue_.push(w.first, dist_source_[w.first], 1);
					prev_source_[w.first] = vertex_id;
				}
			}
		}
		else {

			// Relaxes the incoming edges to the vertex.
			for (const auto& v : vertices_->at(vertex_id).in_edges) {
				
				// We only relax edges leading to a higher priority vertex.
				if (visited_target_.find(v.first) != visited_target_.end() || vertices_->at(v.first).order < vertices_->at(vertex_id).order) {

					continue;
				}

				if (dist_target_.find(v.first) == dist_target_.end() || dist_target_[v.first] > dist_target_[vertex_id] + v.second) {

					dist_target_[v.first] = dist_target_[vertex_id] + v.second;
					queue_.push(v.first, dist_target_[v.first], 0);
					prev_target_[v.first] = vertex_id;
				}
			}
		}
	}
	else {

		if (!backward) {

			// Relaxes the outgoing egdes to the vertex.
			for (const auto& w : vertices_->at(vertex_id).out_edges) {

				if (visited_source_.find(w.first) != visited_source_.end()) {

					continue;
				}

				if (dist_source_.find(w.first) == dist_source_.end() || dist_source_[w.first] > dist_source_[vertex_id] + w.second) {

					dist_source_[w.first] = dist_source_[vertex_id] + w.second;
					queue_.push(w.first, dist_source_[w.first], 1);
					prev_source_[w.first] = vertex_id;
				}
			}
		}
		else {

			// Relaxes the incoming edges to the vertex.
			for (const auto& v : vertices_->at(vertex_id).in_edges) {

				if (visited_target_.find(v.first) != visited_target_.end()) {

					continue;
				}

				if (dist_target_.find(v.first) == dist_target_.end() || dist_target_[v.first] > dist_target_[vertex_id] + v.second) {

					dist_target_[v.first] = dist_target_[vertex_id] + v.second;
					queue_.push(v.first, dist_target_[v.first], 0);
					prev_target_[v.first] = vertex_id;
				}
			}
		}
	}
}

std::vector<unsigned long long> bidirectional_search::reconstruct_path(const unsigned long long source, const unsigned long long target, const unsigned long long intersection) {

	std::vector<unsigned long long> path, path_source, path_target;
	path_source.reserve(prev_source_.size());
	path_target.reserve(prev_target_.size());
	path.reserve(prev_source_.size() + prev_target_.size());

	if (!intersection) {

		path.push_back(source);
		path.push_back(target);

		return path;
	}
	else {

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
}
		
std::vector<unsigned long long> bidirectional_search::unpack_shortcut(unsigned long long start, unsigned long long end) {

	std::vector<unsigned long long> stack;
	std::vector<unsigned long long> unpacked_shortcuts;
	stack.push_back(end);

	while (!stack.empty()) {

		auto next_id = stack.back();

		if (is_shortcut(start, next_id)) {

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

std::vector<unsigned long long> bidirectional_search::unpack_path(const std::vector<unsigned long long>* shortest_path) {

	int i = 0;
	std::vector<unsigned long long> path;
	path.push_back(shortest_path->at(0));

	// Unpacks all shortcuts in the path.
	while (i < shortest_path->size() - 1) {

		if (is_shortcut(shortest_path->at(i), shortest_path->at(i + 1))) {

			auto unpacked_shortcuts = unpack_shortcut(shortest_path->at(i), shortest_path->at(i + 1));
			path.insert(path.end(), unpacked_shortcuts.begin(), unpacked_shortcuts.end());
		}
		else {

			path.push_back(shortest_path->at(i + 1));
		}
		
		i++;
	}

	return path;
}

bool bidirectional_search::is_shortcut(const unsigned long long start, const unsigned long long end) {

	if (shortcuts_->find(start) != shortcuts_->end() && shortcuts_->at(start).find(end) != shortcuts_->at(start).end()) {

		return true;
	}
	else {

		return false;
	}
}
