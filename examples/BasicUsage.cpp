#include "RoutingEngine.h"

int main() {
    // Upon initialization, the engine will parse the provided file, construct a graph that represents a road network, and contract the graph.
    OSM::RoutingEngine engine("example_map_data.osm", true, "minutes", "miles", true);


    // Determines the shortest path between the provided OSM node IDs. Returns the path and travel time as a pair.
    auto routing_data = engine.computeRoute(151368922, 7168173885);

    // Print the routing data to the console.
    std::cout << "Path: [ ";
    for (const auto& coordinates : routing_data.first) {
        std::cout << "( " << coordinates[0] <<", " << coordinates[1] << ") ";
    }
    std::cout << "]\n\nTravel Time: " << routing_data.second << " minutes" << std::endl;

    return 0;
}
