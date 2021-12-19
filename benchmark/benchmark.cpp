#include "benchmark.h"
#include "../src/Serialization/serialize.h"
#include <random>
#include <iostream>

// Error tolerance for shortest path distance.
static const double epsilon = 0.00001;

 void timer::start() {

	start_time_ = std::chrono::system_clock::now();
	running_ = true;
}

void timer::stop() {

	end_time_ = std::chrono::system_clock::now();
	running_ = false;
}

double timer::elapsed_milliseconds() {

	std::chrono::time_point<std::chrono::system_clock> endTime;

	if (running_) {

		endTime = std::chrono::system_clock::now();
	}
	else {

		endTime = end_time_;
	}

	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - start_time_).count();
}

double timer::elapsed_seconds() {

	return elapsed_milliseconds() / 1000.0;
}

void test::test_bidirectional_search(graph graph1, graph graph2, int number_of_tests) {

	auto vertices = graph1.get_vertices();
	bool passed_testing = true;
	int number_tests_conducted = 0;
	int number_tests_failed = 0;

	std::cout << "Testing bidirectional search algorithm..." << std::endl;

	while (number_tests_conducted < number_of_tests) {

		// Getting a random start point in the graph.
		auto random_it1 = std::next(std::begin(vertices), rand() % vertices.size());
		auto random_start = (*random_it1).first;

		// Getting a random end point in the graph.
		auto random_it2 = std::next(std::begin(vertices), rand() % vertices.size());
		auto random_end = (*random_it2).first;

		auto path1 = graph1.get_shortest_path(random_start, random_end, true);
		auto path2 = graph2.get_shortest_path(random_start, random_end);

		// Paths contain different number of nodes.
		if (path1.first.size() != path2.first.size() && std::abs(path1.second - path2.second) > epsilon) {

			std::cout << "----TEST-FAILED----" << std::endl;
			std::cout << "Source: " << random_start << " Target: " << random_end << std::endl;
			std::cout << "Paths are different sizes." << std::endl;
			std::cout << "Expected size: " << path1.first.size() << " Result size: " << path2.first.size() << std::endl;
			std::cout << "-------------------\n\n" << std::endl;

			passed_testing = false;
			number_tests_failed++;
			continue;
		}
		
		int i = 0;

		// Checks to see if paths contain different nodes.
		while(i < path1.first.size() && i < path2.first.size()) {
			
			if (path1.first[i] != path2.first[i] && std::abs(path1.second - path2.second) > epsilon) {

				std::cout << "----TEST-FAILED----" << std::endl;
				std::cout << "Source: " << random_start << " Target: " << random_end << std::endl;
				std::cout << "Paths contain different nodes." << std::endl;
				std::cout << "Expected node: " << path1.first[i] << " Result node: " << path2.first[i] << std::endl;
				std::cout << "-------------------\n\n" << std::endl;

				number_tests_failed++;
				passed_testing = false;
			}

			i++;
		}
		
		number_tests_conducted++;
	}

	std::cout << "Testing complete. Tested " << number_of_tests << " routes." << std::endl;

	if (passed_testing) {

		std::cout << "Passed all test cases." << std::endl;
	}
	else {

		std::cout << "Failed " << number_tests_failed << " tests." << std::endl;
	}
}

void test::test_avg_routing_time(graph graph, int number_of_tests) {

	auto vertices = graph.get_vertices();
	int number_tests_conducted = 0;
	double avg_time = 0.0;
	timer timer; 

	std::cout << "Testing average routing time..." << std::endl;

	while (number_tests_conducted < number_of_tests) {

		// Getting a random start point.
		auto random_it1 = std::next(std::begin(vertices), rand() % vertices.size());
		auto random_start = (*random_it1).first;

		// Getting a random end point.
		auto random_it2 = std::next(std::begin(vertices), rand() % vertices.size());
		auto random_end = (*random_it2).first;
		
		// Timing the execution time of the bidrectional search.
		timer.start();
		graph.get_shortest_path(random_start, random_end);
		timer.stop();

		avg_time += timer.elapsed_milliseconds();

		number_tests_conducted++;
	}

	avg_time = avg_time / number_of_tests;

	std::cout << "Testing complete. Tested " << number_of_tests << " routes." << std::endl;

	std::cout << "Average routing time is " << avg_time << " milliseconds." << std::endl;
}