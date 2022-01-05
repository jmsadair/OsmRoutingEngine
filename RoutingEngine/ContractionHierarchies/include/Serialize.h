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
	
	/**
	 * Serializes an object and saves it the the provided filename.
	 * @tparam T The type of the object to be serialized. See cereal documentation.
	 * @param filename The name of the file that the serialized object should be saved to.
	 * @param object The object to be serialized.
	 */
    template <class T>
    static void save(const char* filename, T object) {
        std::ofstream os(filename, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(object);
    }

    /**
     * Loads a serialized object from the given filename.
     * @tparam T The type of the object to be loaded. See cereal documentation.
     * @param filename The filename of the file to be loaded.
     * @return The object that is loaded from the given filename.
     */
    template <class T>
    static T load(const char* filename) {
        T object;
        std::ifstream is(filename, std::ios::binary);
        cereal::BinaryInputArchive archiveLoad(is);
        archiveLoad(object);
        return object;
    }
};