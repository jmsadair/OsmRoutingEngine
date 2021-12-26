#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <array>
#include "pugixml.hpp"
#include "Graph.h"

// The way struct is used to store all the basic information found in an OSM way.
struct Way {
    /**
    * Contains all of the node references present in a way. Node references are ID numbers
    * that corresponds to a unique coordinate pair. Noe that node references are 64 but unsigned
    * integers >= 1, and so an unsigned long long is necessary.
    */
    std::vector<uint64_t> node_refs;

    /**
    * Contains all of the tags present in a way. Tags consist of a key-value pair. Tags describe
    * features of map elements. For example, highway=residential is a key-value pair.
    */
    std::unordered_map<std::string, std::string> tags;

    Way(std::vector<uint64_t>   way_node_refs, std::unordered_map<std::string, std::string>   way_tags);
};

/**
* The primary purpose of this class is to gather the relevant data for route planning from an OpenStreetMaps (OSM) file.
* Note that an OSM file is an identical to an XML file. See https://www.openstreetmap.org/#map=18/30.26889/-97.74373 in
* order to download OSM data.
*/
class Parser {

private:

    // Many times, OSM files do not include speed limits. We use default speed limits instead.
    const std::unordered_map<std::string, int> DEFAULT_SPEED_MPH;

    // Way tags that are important for preparing the routing data.
    const std::unordered_set<std::string> ACCEPTED_TAGS;

    pugi::xml_document doc;

public:

    explicit Parser(const char* osm_filename);

    /**
    * This function will retrieve the IDs and coordinates of all the nodes in the OSM file.
    */
    std::unordered_map<uint64_t, std::array<double, 2>> getLocations() const;

    /**
    * This function will retrieve the node references and tags from all the ways in the
    * OSM file.
    */
    std::vector<Way> getAllWays() const;

    /**
     * This function will only retrieve the ways and locations that will be used in the road network graph.
     * Returns a pair that contains the ways and locations.
     */
    std::tuple<std::vector<Way>, std::unordered_map<uint64_t, std::array<double, 2>>, std::unordered_map<uint64_t, int>> getRoutingData() const;

    /**
    * This function will construct a weighted, directed graph from the data in the OSM file.
    * Nodes that are present in more than one way are intersections and will be used as vertices
    * in the graph. Edges are made up of all of the node IDs that connect two intersection. Each
    * Edge is assigned a time weight (in minutes). The graph, as well as an unordered map of the
    * locations is returned.
    */
    std::pair<Graph, std::unordered_map<uint64_t, std::array<double, 2>>> constructRoadNetworkGraph() const;
};