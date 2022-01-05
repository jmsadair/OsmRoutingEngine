#include "RoutingEngine.h"

using namespace OSM;

RoutingEngine::RoutingEngine(const char *filename, bool time, const std::string &time_units,
                             const std::string &distance_units, bool contracted) {
    // Parses the OSM file.
    Parser parser(filename);
    auto routing_data = parser.constructRoadNetworkGraph(time, time_units, distance_units);

    routing_graph = *std::make_unique<Graph>(routing_data);

    // Contracts the graph.
    if (contracted) {
        HierarchyConstructor builder(routing_graph, 170, 190);
        builder.contractGraph();
    }
}

RoutingEngine::RoutingEngine(const char* filename) : routing_graph(*std::make_unique<Graph>(Serialize::load<Graph>(filename))) {}

RoutingEngine::RoutingEngine() = default;

void RoutingEngine::saveRoutingData(const char *filename) {
    Serialize::save(filename, routing_graph);
}

void RoutingEngine::loadRoutingData(const char *filename) {
    routing_graph = *std::make_unique<Graph>(Serialize::load<Graph>(filename));
}

std::pair<std::vector<std::array<double, 2>>, double> RoutingEngine::computeRoute(uint64_t source, uint64_t target, bool standard) {
    auto routing_data = routing_graph.getShortestPath(source, target, standard);
    return std::make_pair(routing_graph.convertPathToCoordinates(routing_data.first), routing_data.second);
}


