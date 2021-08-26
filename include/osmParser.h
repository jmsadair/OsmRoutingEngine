#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <array>
#include "pugixml.hpp"
#include "graph.h"


// The way struct is used to store all of the basic information found in an OSM way.
struct way {

/**
* Contains all of the node references present in a way. Node references are ID numebers
* that correspons to a unique coordinate pair. Noe that node references are 64 but unsigned
* integers >= 1, and so an unsigned long long is necessary.
*/
	std::vector<unsigned long long> node_refs;

/**
* Contains all of the tags present in a way. Tags consist of a key-value pair. Tags describe
* features of map elements. For example, highway=residential is a key-value pair.
*/
	std::unordered_map<std::string, std::string> tags;

	way(const std::vector<unsigned long long>& way_node_refs, const std::unordered_map<std::string, std::string>& way_tags);
};

/**
* The primary purpose of this class is to gather the relevant data for route planning from an OpenStreetMaps (OSM) file.
* Note that an OSM file is an identical to an XML file. See https://www.openstreetmap.org/#map=18/30.26889/-97.74373 in
* order to download OSM data.
*/
class parser {

private:

// Many times, OSM files do not include speed limits. We use default speed limits instead.
	const std::unordered_map<std::string, int> DEFAULT_SPEED_MPH;
	
// Way tags that are important for preparing the routing data.
	const std::unordered_set<std::string> ACCEPTED_TAGS;

	pugi::xml_document doc;

public:

	parser(const char* osm_filename);

/**
* This function will retreive the IDs and coordinates of all the nodes in the OSM file.
*/
	std::unordered_map<unsigned long long, std::array<double, 2>> get_locations()const;

/**
* This function will retreive the the node references and tags from all the ways in the
* OSM file.
*/
	std::vector<way> get_ways()const;

/**
* This function will construct a weighted, undirected graph from the data in the OSM file.
* Nodes that are present in more than one way are intersections and will be used as vertices
* in the graph. Edges are made up of all of the node IDs that connect two intersection. Each
* edge is assigned a time weight (in minutes). The graph, as well as an unordered map of the
* locations is returned. 
*/
	std::pair<graph, std::unordered_map<unsigned long long, std::array<double, 2>>> get_routing_data()const;
};