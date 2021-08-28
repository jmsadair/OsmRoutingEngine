#pragma once
#include "graph.h"
#include <chrono>

// The purpose of this class is to test the correctness and execution time of the contraction hierarchie routing algorithm.
class test {

public:

	/**
	* Used to test the correctness of the contraction hierarchy bidirectional search. We randomly select vertices in the graph,
	* find the shortest path distance using the contraction hierarchy bidirectional search, find the shortest path using the 
	* bidirectional Dijkstra search, and compare these distances. If they are different, then the test for that pair of vertices
	* has failed. Typically, testing is only conducted on smaller graphs (i.e. graphs that represent a city), as testing larger
	* graphs is extremely slow. We usually run 1000 - 10000 tests depending on the size of the graph.
	*/
	static void test_bidirectional_search(graph graph1, graph graph2, int number_of_tests);

	/**
	* This method determines an average execution time for the contraction hierarchy bidirectional search. Random vertices are
	* selected and the time it takes to find a shortest path bewteen those vertices is recorded. This test is much faster than the above test,
	* and so we can run this test on much larger graphs (i.e. graphs that represent a region of the country). Typically, we conduct 
	* 1000-1000000 tests to get an average execution time. This execution time includes finding a shortest path as well as the 
	* time it takes to unpack that path.
	*/
	static void test_avg_routing_time(graph graph, int number_of_tests);
};

// This is a basic timer class that is used for benchmarking.
class timer {

private:

	std::chrono::time_point<std::chrono::system_clock> start_time_;

	std::chrono::time_point<std::chrono::system_clock> end_time_;

	bool running_ = false;

public:

	void start();

	void stop();

	double elapsed_milliseconds();

	double elapsed_seconds();
};