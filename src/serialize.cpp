#include "serialize.h"

void serialize::save_graph(const char* filename, graph graph) {

	std::ofstream os(filename, std::ios::binary);
	cereal::BinaryOutputArchive archive(os);
	archive(graph);
}

graph serialize::load_graph(const char* filename) {

	graph graph;

	std::ifstream is(filename, std::ios::binary);
	cereal::BinaryInputArchive archiveLoad(is);
	archiveLoad(graph);

	return graph;
}
