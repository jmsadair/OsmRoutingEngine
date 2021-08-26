#pragma once
#include "graph.h"

/**
* The purpose of this class is to provide a way of saving a graph for future use, as parsing and preproccesing
* a very large OSM file can be quite time comsuming. We use the Cereal library to serialize the graph. More information 
* about Cereal can be found at https://uscilab.github.io/cereal/.
*/
class serialize {

public:
	
// Saves the graph as binary file.
	static void save_graph(const char* filename, graph graph);

// Loads a graph that is saved in a binary file.
	static graph load_graph(const char* filename);
};