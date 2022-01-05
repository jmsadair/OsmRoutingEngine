#ifndef OSMROUTINGENGINE_ROUTINGENGINE_H
#define OSMROUTINGENGINE_ROUTINGENGINE_H
#include <string>
#include <memory>
#include "Serialize.h"
#include "Graph.h"
#include "HierarchyConstructor.h"
#include "OsmParser.h"

namespace OSM {
    class RoutingEngine {

    private:

        // The road network graph that will be used for routing.
        Graph routing_graph;

    public:

        /**
         * A constructor for the RoutingEngine class.
         * @param filename The filename of the OSM file that will be parsed and converted into a road network graph.
         * @param time A boolean value indicating whether time units will serve as the primary edge weight in the road
         * graph. If time is set to true, then time units will be used. Otherwise, distance units will be used.
         * @param time_units The type of time units to be used (i.e. "seconds", "minutes", or "hours").
         * @param distance_units The type of distance units to be used (i.e. "kilometers" or "miles").
         * @param contract A boolean indicating whether the road network should be contracted or not. If true, then the
         * graph will be contracted. If false, the graph will not be contracted.
         */
        explicit RoutingEngine(const char *filename, bool time = true, const std::string &time_units = "minutes",
                               const std::string &distance_units = "miles", bool contract = true);

        /**
         * A constructor for the RoutingEngine class.
         * @param filename The filename of a binary file that contains a routing graph.
         */
        explicit RoutingEngine(const char* filename);

        RoutingEngine();

        /**
         * Saves the routing graph as a binary file.
         * @param filename The name of the file to save the graph to.
         */
        void saveRoutingData(const char *filename);

        /**
         * Loads the routing graph from the provided filename.
         * @param filename The name of the binary file to load the graph from.
         */
        void loadRoutingData(const char *filename);

        /**
         * Computes the route between two points given an as OSM node IDs.
         * @param source The OSM Node ID that will serve as the start point in the route.
         * @param target The OSM Node ID that will serve as the end point of the route.
         * @param standard If standard is true, a bidirectional Dijkstra search algorithm will be used to compute the
         * route rather than the modified contraction hierarchies search algorithm.
         * @return A pair containing the optimal route from the source to the target as well as distance/time cost
         * of that route.
         */
        std::pair<std::vector<std::array<double, 2>>, double> computeRoute(uint64_t source, uint64_t target, bool standard = false);
    };
}
#endif //OSMROUTINGENGINE_ROUTINGENGINE_H
