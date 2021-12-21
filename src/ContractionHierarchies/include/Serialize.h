#pragma once
#include <fstream>
#include "cereal/archives/binary.hpp"
/**
* The purpose of this class is to provide a way of saving a graph for future use, as parsing and preprocessing
* a very large OSM file can be quite time consuming. We use the Cereal library to Serialize the graph. More information
* about Cereal can be found at https://uscilab.github.io/cereal/.
*/
class Serialize {

public:
	
	// Saves the graph as binary file.
    template <class T>
    static void save(const char* filename, T object) {
        std::ofstream os(filename, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(object);
    }

    template <class T>
    static T load(const char* filename) {
        T object;
        std::ifstream is(filename, std::ios::binary);
        cereal::BinaryInputArchive archiveLoad(is);
        archiveLoad(object);
        return object;
    }
};