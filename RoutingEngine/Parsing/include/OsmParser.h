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

    // Contains all the node references present in a way. Node references are ID numbers
    // that corresponds to a unique coordinate pair. Noe that node references are 64 but unsigned
    // integers >= 1, and so an unsigned long long is necessary.
    std::vector<uint64_t> node_refs;


    // Contains all the tags present in a way. Tags consist of a key-value pair. Tags describe
    // features of map elements. For example, highway=residential is a key-value pair.
    std::unordered_map<std::string, std::string> tags;

    /**
     * A constructor for the Way struct.
     * @param way_node_refs The OSM node IDs that make up the way.
     * @param way_tags The tags that correspond to the way (i.e. "highway", "maxspeed", "oneway", etc.).
     */
    Way(std::vector<uint64_t>   way_node_refs, std::unordered_map<std::string, std::string>   way_tags);
};

/**
* The primary purpose of this class is to gather the relevant data for route planning from an OpenStreetMaps (OSM) file.
* Note that an OSM file is an identical to an XML file. See https://www.openstreetmap.org/#map=18/30.26889/-97.74373 in
* order to download OSM data.
*/
class Parser {

private:

    // Many times, OSM files do not include speed limits. We use default speed limits if one is not provided.
    const std::unordered_map<std::string, int> DEFAULT_SPEED_MPH{
                                                                     {"motorway" , 60},
                                                                     {"trunk" , 45},
                                                                     {"primary" , 35},
                                                                     {"secondary" , 30},
                                                                     {"residential" , 25},
                                                                     {"tertiary" , 25},
                                                                     {"unclassified" , 25},
                                                                     {"living_street" , 10},
                                                                     {"motorway_link" , 30},
                                                                     {"trunk_link" , 30},
                                                                     {"primary_link" , 30},
                                                                     {"secondary_link" , 30},
                                                                     {"tertiary_link" , 25},
                                                                     {"service" , 10},
                                                                     {"road" , 35},
                                                                     {"pedestrian" , 10},
                                                                     {"track", 15},
                                                                     {"bus_guideway", 40},
                                                                     {"escape", 10},
                                                                     {"busway", 35},
                                                                 };

    // Way tags that are important for preparing the routing data.
    const std::unordered_set<std::string> ACCEPTED_TAGS{ "highway", "oneway", "maxspeed"};

    // The OSM file to be parsed.
    pugi::xml_document doc;

public:

    /**
     * A constructor for the Parser class.
     * @param osm_filename The name of the file to be parsed.
     */
    explicit Parser(const char* osm_filename);

    /**
     * This function will retrieve the IDs and coordinates of all the nodes in the OSM file.
     * @return An unordered map that maps OSM Node IDs to coordinates.
     */
    std::unordered_map<uint64_t, std::array<double, 2>> getLocations() const;

    /**
     * This function will retrieve the node references and tags from all the ways in the
     * OSM file.
     * @return A vector of Ways.
     */
    std::vector<Way> getAllWays() const;

    /**
     * This function will only retrieve the ways and locations that will be used in the road network graph.
     * @return a tuple that contains the ways and locations that will be used for routing, as well as an unordered map
     * that maps OSM Node IDs to the number of times that they appear in the ways (used for way splitting).
     */
    std::tuple<std::vector<Way>, std::unordered_map<uint64_t, std::array<double, 2>>, std::unordered_map<uint64_t, int>> getRoutingData() const;

    /**
     * This function will construct a weighted, directed graph from the data in the OSM file.
     * Nodes that are present in more than one way are intersections and will be used as vertices
     * in the graph. Edges are made up of all of the node IDs that connect two intersection. Each
     * Edge is assigned a weight that is in terms of time or distance.
     * @param time A boolean that indicates whether time units will serve as the primary edge weights in the road
     * network graph. If true, time units will be used. If false, distance units will be used.
     * @param time_units The type of time units to use (i.e. "seconds", "minutes", or "hours").
     * @param distance_units The type of distance units to use (i.e. "kilometers" or "miles").
     * @return A road network graph that can be used for routing.
     */
    Graph constructRoadNetworkGraph(bool time = true, const std::string& time_units = "minutes", const std::string& distance_units = "miles") const;
};