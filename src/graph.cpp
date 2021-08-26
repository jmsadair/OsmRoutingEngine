#include "bidirectionalSearch.h"
#include "graph.h"


vertex::vertex(const unsigned long long id, unsigned long long order, bool contracted)
	: id(id), order(order), deleted_neighbors(0)
{}

vertex::vertex()
	:id(0), order(0), deleted_neighbors(0)
{}

edge::edge(unsigned long long start, unsigned long long end, std::vector<unsigned long long>& nodes, double weight)
	: start(start), end(end), nodes(nodes), weight(weight)
{}

edge::edge()
	: start(0), end(0), weight(0)
{}

graph::graph()
	:num_edges_(0)
{}

void graph::add_edge(const unsigned long long start, const unsigned long long end, std::vector<unsigned long long>& nodes, const double weight, const bool bidirectional) {

	if (vertices.find(start) == vertices.end()) {

		vertices.emplace(start, start);
	}

	if (vertices.find(end) == vertices.end()) {

		vertices.emplace(end, end);
	}

	edges[start][end] = edge(start, end, nodes, weight);

	vertices[start].out_edges[end] = weight;
	vertices[end].in_edges[start] = weight;
	num_edges_++;

	if (bidirectional) {

		std::reverse(nodes.begin(), nodes.end());
		edges[end][start] = edge(end, start, nodes, weight);

		vertices[start].in_edges[end] = weight;
		vertices[end].out_edges[start] = weight;
		num_edges_++;
	}
}

bool graph::edge_exists(const unsigned long long start, const unsigned long long end) {

	if (vertices.find(start) != vertices.end() && vertices[start].out_edges.find(end) != vertices[start].out_edges.end()) {

		return true;
	}
	else {

		return false;
	}
}

void graph::remove_edge(unsigned long long start, unsigned long long end) {

	if (edge_exists(start, end)) {

		vertices[start].out_edges.erase(end);
		vertices[end].in_edges.erase(start);
	}

}
void graph::add_shortcut(const unsigned long long start, const unsigned long long end, const unsigned long long through, const double weight) {

	shortcuts[start][end] = through;
	vertices[start].out_edges[end] = weight;
	vertices[end].in_edges[start] = weight;
	num_edges_++;
}

void graph::optimize_edges() {

	// Loops through all edges in the graph and removes those that will never appear on the shortest path.
	for (const auto& vertex : vertices) {

		auto it = vertices[vertex.first].out_edges.begin();

		while (it != vertices[vertex.first].out_edges.end()) {

			if (vertices[vertex.first].order > vertices[std::get<0>(*it)].order) {

				it = vertices[vertex.first].out_edges.erase(it);
			}
			else {

				++it;
			}
		}

		it = vertices[vertex.first].in_edges.begin();

		while (it != vertices[vertex.first].in_edges.end()) {

			if (vertices[vertex.first].order > vertices[std::get<0>(*it)].order) {

				it = vertices[vertex.first].in_edges.erase(it);
			}
			else {

				++it;
			}
		}
	}
}

void graph::add_ordering(const unsigned long long vertex, const unsigned long long ordering) {

	if (vertices.find(vertex) != vertices.end()) {

		vertices[vertex].order = ordering;
	}
}

std::pair<std::vector<unsigned long long>, double> graph::get_shortest_path(const unsigned long long source, const unsigned long long target, const bool standard) {

	bidirectional_search searcher(&vertices, &shortcuts);
	
	// If standard is set to true, then a stanadrd bidirectional Dijkstra search is performed. The standard search is primarily used for testing.
	if (standard) {

		return searcher.execute_standard_search(source, target);
	}
	else {

		return searcher.execute_search(source, target);
	}
}
