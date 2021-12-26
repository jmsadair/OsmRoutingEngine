#include "HierarchyConstructor.h"
#include "OsmParser.h"
#include "Serialize.h"

/**
* This is a basic demonstration of how one might use CppRoutingEngine. The code below transforms an OSM file
* (city of Bellaire, TX) into a routing hierarchy, tests the average routing time, finds a route between
* two vertices in the graph, and saves the graph.
*/
int main() {
    // Takes an osm file as an argument.
    Parser osm("example_map_data.osm");

    // Returns a pair containing a graph object and unordered map containing node IDs as keys and coordinates as values
    auto example_routing_data = osm.constructRoadNetworkGraph();

    // Takes a graph object as an argument.
    HierarchyConstructor builder(example_routing_data.first);

    // Transforms graph into a routing hierarchy.
    builder.contractGraph();

    // Finds the shortest path between two vertices in the graph.
    example_routing_data.first.getShortestPath(3939562845, 1239440973);

    // Saves the graph in a binary file to avoid parsing and preprocessing the data in the future.
    Serialize::save("example_graph.bin", example_routing_data.first);

    return 0;
}

