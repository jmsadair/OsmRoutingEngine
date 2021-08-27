#include "hierarchyConstructor.h"
#include "osmParser.h"
#include "graph.h"
#include "serialize.h"
#include "tests.h"

/**
* This is a baic demonstration of how one might use CppRoutingEngine. The code below transforms an OSM file
* (city of bellaire, TX) into a routing hierarchy, tests the average routing time, finds a route bewteen
* two vertices in the graph, and saves the graph.
*/
int main() {

	// Takes an osm file as an argument.
	parser osm("example_map_data.osm");

	// Returns a pair containing a graph object and unordered map containing node IDs as keys and coordinates as values
	auto example_routing_data = osm.get_routing_data();
	
	// Takes a graph object as an argument.
	constructor hierarchy_builder(example_routing_data.first);

	// Transforms graph into an a routing hierarchy.
    hierarchy_builder.contract_graph();

	// Tests average routing time by randomly selecting vertices in the graph. Conducts 1000 tests. If graph is small, average routing time is typically 0 ms.
	test::test_avg_routing_time(example_routing_data.first, 1000);

	// Computes the shortest path bewteen two vertices in the graph.
	auto path = example_routing_data.first.get_shortest_path(152128093, 2133963304);
	
	// Saves the graph in a binary file to avoid parsing and preprocessing the data in the future.
	serialize::save_graph("example_graph.bin", example_routing_data.first);
	
	return 0;
}
	
