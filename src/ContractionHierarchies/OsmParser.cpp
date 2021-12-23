#include "Weighting.h"
#include "OsmParser.h"
#include <utility>

using array1 = std::array<double, 2>;

Way::Way(std::vector<uint64_t> way_node_refs, std::unordered_map<std::string, std::string> way_tags)
        : node_refs(std::move(way_node_refs)), tags(std::move(way_tags))
{}

Parser::Parser(const char* osm_filename)
        : ACCEPTED_TAGS({ "highway", "oneway"}),
          DEFAULT_SPEED_MPH({
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
                            })
{
    if (!doc.load_file(osm_filename)) { throw std::runtime_error("Could not open OSM file."); }
}

std::unordered_map<uint64_t , array1> Parser::get_locations() const {
    std::unordered_map<uint64_t, array1> locations;
    array1 coords;
    const auto xpath_nodes = doc.select_nodes("/osm/node");
    locations.reserve(xpath_nodes.size());

    // Loops through all the nodes in the data. Records their ID and coordinates.
    for (pugi::xpath_node xpath_node : xpath_nodes) {
        auto node = xpath_node.node();
        coords[0] = node.attribute("lat").as_double();
        coords[1] = node.attribute("lon").as_double();
        locations[node.attribute("id").as_ullong()] = coords;
    }
    return locations;
}

std::vector<Way> Parser::get_ways() const {
    std::vector<Way> ways;
    const pugi::xpath_node_set xpath_ways = doc.select_nodes("/osm/way");
    ways.reserve(xpath_ways.size());

    // Loops through all the ways in the data. Records their node references and any important tags.
    for (const auto& xpath_way : xpath_ways) {
        std::unordered_map<std::string, std::string> way_tags;
        std::vector<uint64_t> way_node_refs;
        pugi::xml_node way = xpath_way.node();
        way_node_refs.reserve(std::distance(way.children("nd").begin(), way.children("nd").end()));
        // Getting the node references.
        for (const auto& node : way.children("nd")) {
            way_node_refs.push_back(node.attribute("ref").as_ullong());
        }
        // Getting the tags.
        for (const auto& tag : way.children("tag")) {
            std::string tag_key = tag.attribute("k").value(), tag_value = tag.attribute("v").value();
            way_tags[tag_key] = tag_value;
        }
        ways.emplace_back(way_node_refs, way_tags);
    }
    return ways;
}

std::pair<Graph, std::unordered_map<uint64_t, std::array<double, 2>>> Parser::get_routing_data() const {
    Graph graph;
    std::vector<uint64_t> edge_nodes;
    std::vector<Way> ways;
    std::unordered_map<uint64_t, int> node_links;
    auto locations = get_locations();
    auto xpath_ways = doc.select_nodes("/osm/way");
    node_links.reserve(locations.size());
    ways.reserve(xpath_ways.size());

    /**
    * Loops through all of the ways in the data. Records their node references and any important tags.
    * This is essentially identical to the get_ways method. However, in this case, we throw out any ways
    * that are not useful for routing and only record accepted tags.
    */
    for (const auto& xpath_way : xpath_ways) {
        std::unordered_map<std::string, std::string> way_tags;
        std::vector<uint64_t> way_node_refs;
        auto way = xpath_way.node();
        way_node_refs.reserve(std::distance(way.children("nd").begin(), way.children("nd").end()));

        for (const auto& node : way.children("nd")) {
            // We ignore any node references that do not have a corresponding node.
            if (locations.find(node.attribute("ref").as_ullong()) != locations.end()) {
                way_node_refs.push_back(node.attribute("ref").as_ullong());
            }
        }

        // We ignore any ways that contain less than two nodes.
        if (way_node_refs.size() < 2) { continue; }

        /**
        * We need to record how many times a node is seen for the splitting process later on.
        * A node that is seen more than once is an intersection and will be used as a Vertex in the graph data structure.
        */
        for (const auto& node_ref : way_node_refs) {
            node_links[node_ref]++;
        }
        for (const auto& tag : way.children("tag")) {
            std::string tag_key = tag.attribute("k").value(), tag_value = tag.attribute("v").value();
            if (ACCEPTED_TAGS.find(tag_key) != ACCEPTED_TAGS.end()) { way_tags[tag_key] = tag_value; }
        }

        // If the way has no highway tag, then it cannot be used for routing.
        if (way_tags.find("highway") != way_tags.end()) { ways.emplace_back(way_node_refs, way_tags); }
    }

    /**
    * We now split the ways into edges that will be used in a weighted, directed graph. A split is made if a node is present in more than one
    * way (i.e. an intersection).
    */
    int counter = 0;
    for (const auto& way : ways) {
        int right_idx = 1;
        int left_idx = 0;
        double weight = 0.0;
        counter++;
        while (right_idx < way.node_refs.size()) {
            int speed_mph = 35;
            if (DEFAULT_SPEED_MPH.find(way.tags.at("highway")) != DEFAULT_SPEED_MPH.end()) {
                speed_mph = DEFAULT_SPEED_MPH.at(way.tags.at("highway"));
            }

            // Travel time serves as the edge weight in the road hierarchy graph.
            weight += Weighting::time(locations[way.node_refs[right_idx - 1]][0], locations[way.node_refs[right_idx - 1]][1],
                                      locations[way.node_refs[right_idx]][0], locations[way.node_refs[right_idx]][1], speed_mph);

            if (node_links[way.node_refs[right_idx]] > 1 || right_idx == way.node_refs.size() - 1) {
                uint64_t start = way.node_refs[left_idx];
                uint64_t end = way.node_refs[right_idx];
                edge_nodes = std::vector<uint64_t>(way.node_refs.begin() + left_idx - 1, way.node_refs.begin() + right_idx );
                // Checking if Edge is bidirectional.
                if (way.tags.find("oneway") == way.tags.end() || way.tags.at("oneway") == "no") {
                    graph.addEdge(start, end, &edge_nodes, weight, true);
                }
                else {
                    graph.addEdge(start, end, &edge_nodes, weight);
                }
                left_idx = right_idx;
                weight = 0.0;
            }
            right_idx++;
        }
    }
    return std::make_pair(graph, locations);
}
