#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Queue.h"
#include "Graph.h"
#include "OsmParser.h"
#include "HierarchyConstructor.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include <fstream>

TEST_CASE( "Queue pop/push test", "[Queue]") {
    Queue<int> Q1;
    Q1.push(1);
    Q1.push(2);
    Q1.push(3);
    REQUIRE( Q1.pop() == 1 );
    REQUIRE( Q1.pop() == 2 );
    REQUIRE( Q1.pop() == 3 );

    Queue<float> Q2;
    Q2.push(1.001);
    Q2.push(2.647);
    Q2.push(17.3454);
    Q2.push(0.345);
    Q2.push(4.54553);
    REQUIRE( Q2.pop() == 0.345f );
    REQUIRE( Q2.pop() == 1.001f );
    REQUIRE( Q2.pop() == 2.647f );
    REQUIRE( Q2.pop() == 4.54553f );
    REQUIRE( Q2.pop() == 17.3454f );

    Queue<HeapElement> Q3;
    Q3.push(HeapElement(1, 1, 1));
    Q3.push(HeapElement(2, 18, 1));
    Q3.push(HeapElement(3, 0.5, 0));
    REQUIRE( Q3.pop().value == 0.5f );
    REQUIRE( Q3.pop().value == 1 );
    REQUIRE( Q3.pop().value == 18 );

    Queue<int> Q4;
    int num = 0;
    REQUIRE_THROWS_AS( Q4.pop(), std::logic_error );
    REQUIRE_THROWS_AS( Q4.lazyUpdate(1), std::logic_error );
    for (int i = 100; i >= 0; i--) {
        Q4.push(i);
    }
    while(!Q4.empty()) {
        REQUIRE( Q4.pop() == num );
        num++;
    }
}

TEST_CASE( "Queue empty test", "[Queue]") {
    Queue<int> Q1;
    REQUIRE( Q1.empty() == true );
    Q1.push(1);
    Q1.push(2);
    REQUIRE( Q1.empty() == false );
    Q1.pop();
    REQUIRE( Q1.empty() == false );
    Q1.pop();
    REQUIRE( Q1.empty() == true );
}

TEST_CASE( "Queue size test", "[Queue]") {
    Queue<int> Q1;
    Q1.push(1);
    REQUIRE( Q1.size() == 1 );
    Q1.push(2);
    REQUIRE( Q1.size() == 2 );
    Q1.push(3);
    REQUIRE( Q1.size() == 3 );
    Q1.pop();
    REQUIRE( Q1.size() == 2 );
    Q1.pop();
    REQUIRE( Q1.size() == 1 );
}

TEST_CASE( "Queue peek test", "[Queue]") {
    Queue<int> Q1;
    Q1.push(1);
    Q1.push(2);
    Q1.push(3);
    REQUIRE( Q1.peek() == 1 );
    Q1.pop();
    REQUIRE( Q1.peek() == 2 );
    Q1.pop();
    REQUIRE( Q1.peek() == 3 );
    Q1.pop();
    REQUIRE_THROWS_AS(Q1.peek(), std::logic_error);
}

TEST_CASE( "Queue makeHeap test", "[Queue]") {
    std::vector<int> nums{1, 4, 3, 5, 12, 2, 14, 25, 34 , 8, 900, 123, 27, 78, 14};
    Queue<int> Q1;
    Q1.makeHeap(nums);
    std::sort(nums.begin(), nums.end(), std::greater<int>());
    while (!Q1.empty()) {
        REQUIRE( Q1.pop() == nums.back() );
        nums.pop_back();
    }
}

TEST_CASE( "Large Queue with floats test", "[Queue]") {
    std::vector<float> nums;
    Queue<float> Q1;

    // Read the test input file.
    std::fstream file;
    file.open("test_input4.txt", std::ios::out);
    if (file.is_open()) {
        std::string num;
        while(getline(file, num)){
            nums.push_back(std::stof(num));
        }
        file.close();
    }

    Q1.makeHeap(nums);
    std::sort(nums.begin(), nums.end(), std::greater<int>());
    while (!Q1.empty()) {
        REQUIRE(Q1.pop() == nums.back());
        nums.pop_back();
    }
}

TEST_CASE( "Graph addEdge, edgeExists, and removeEdge test", "[Graph]") {
    Graph graph1;
    graph1.addEdge(1, 4, 5, true);
    graph1.addEdge(5, 9, 10, false);
    REQUIRE(graph1.edgeExists(1, 4) == true );
    REQUIRE(graph1.edgeExists(4, 1) == true );
    REQUIRE(graph1.edgeExists(5, 9) == true );
    REQUIRE(graph1.edgeExists(9, 5) == false );
    graph1.removeEdge(1, 4);
    REQUIRE(graph1.edgeExists(1, 4) == false );
    REQUIRE(graph1.edgeExists(4, 1) == true );
    REQUIRE(graph1.edgeExists(5, 9) == true );
    REQUIRE(graph1.edgeExists(9, 5) == false );
    graph1.removeEdge(4, 1);
    REQUIRE(graph1.edgeExists(1, 4) == false );
    REQUIRE(graph1.edgeExists(4, 1) == false );
    REQUIRE(graph1.edgeExists(5, 9) == true );
    REQUIRE(graph1.edgeExists(9, 5) == false );

    Graph graph2;
    graph2.addEdge(1, 2, 4, true);
    graph2.addEdge(3, 4, 3, true);
    graph2.addEdge(5, 6, 7, true);
    graph2.addEdge(7, 8, 12, true);
    graph2.addEdge(9, 10, 33, false);
    graph2.addEdge(11, 12, 1, false);
    graph2.addEdge(1, 12, 132, false);
    REQUIRE(graph2.edgeExists(1, 2) == true );
    REQUIRE(graph2.edgeExists(2, 1) == true );
    REQUIRE(graph2.edgeExists(3, 4) == true );
    REQUIRE(graph2.edgeExists(4, 3) == true );
    REQUIRE(graph2.edgeExists(5, 6) == true );
    REQUIRE(graph2.edgeExists(6, 5) == true );
    REQUIRE(graph2.edgeExists(7, 8) == true );
    REQUIRE(graph2.edgeExists(8, 7) == true );
    REQUIRE(graph2.edgeExists(9, 10) == true );
    REQUIRE(graph2.edgeExists(10, 9) == false );
    REQUIRE(graph2.edgeExists(11, 12) == true );
    REQUIRE(graph2.edgeExists(12, 11) == false );
    REQUIRE(graph2.edgeExists(1, 12) == true );
    graph2.removeEdge(2, 1);
    graph2.removeEdge(1, 12);
    graph2.removeEdge(7, 8);
    graph2.removeEdge(4, 3);
    REQUIRE(graph2.edgeExists(1, 2) == true );
    REQUIRE(graph2.edgeExists(2, 1) == false );
    REQUIRE(graph2.edgeExists(3, 4) == true );
    REQUIRE(graph2.edgeExists(4, 3) == false );
    REQUIRE(graph2.edgeExists(5, 6) == true );
    REQUIRE(graph2.edgeExists(6, 5) == true );
    REQUIRE(graph2.edgeExists(7, 8) == false );
    REQUIRE(graph2.edgeExists(8, 7) == true );
    REQUIRE(graph2.edgeExists(9, 10) == true );
    REQUIRE(graph2.edgeExists(10, 9) == false );
    REQUIRE(graph2.edgeExists(11, 12) == true );
    REQUIRE(graph2.edgeExists(12, 11) == false );
    REQUIRE(graph2.edgeExists(1, 12) == false );

    Graph graph3;
    graph3.addEdge(1, 2, 3, false);
    REQUIRE_THROWS_AS(graph3.removeEdge(2, 1), std::logic_error );
}

TEST_CASE( "Graph getNumVertices and getNumEdges test", "[Graph]") {
    Graph graph1;
    graph1.addEdge(1, 2, 5, true);
    REQUIRE(graph1.getNumEdges() == 2 );
    REQUIRE(graph1.getNumVertices() == 2 );
    graph1.addEdge(2, 3, 5, false);
    REQUIRE(graph1.getNumEdges() == 3 );
    REQUIRE(graph1.getNumVertices() == 3 );

    Graph graph2;
    graph2.addEdge(1, 2, 3, true);
    REQUIRE(graph2.getNumEdges() == 2 );
    REQUIRE(graph2.getNumVertices() == 2);
    graph2.removeEdge(2, 1);
    REQUIRE(graph2.getNumEdges() == 1 );
    REQUIRE(graph2.getNumVertices() == 2 );

    Graph graph3;
    REQUIRE(graph3.getNumEdges() == 0 );
    REQUIRE(graph3.getNumVertices() == 0 );
}

TEST_CASE( "Simple Bidirectional Dijkstra test", "[BidirectionalSearch]") {
    Graph graph1;
    graph1.addEdge(1, 2, 5, true);
    graph1.addEdge(2, 3, 7, true);
    graph1.addEdge(1, 3, 1, true);
    std::vector<uint64_t> expectedPath{1, 3};
    double expectedWeight = 1;
    auto result = graph1.getShortestPath(1, 3, true);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );
    graph1.addEdge(1, 4, 0.2, true);
    graph1.addEdge(4, 5, 0.3, true);
    graph1.addEdge(5, 3, 0.1, true);
    expectedPath = {1, 4, 5, 3};
    expectedWeight = 0.6;
    result = graph1.getShortestPath(1, 3, true);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph2;
    graph2.addEdge(1, 2, 1, false);
    graph2.addEdge(1, 3, 2, true);
    graph2.addEdge(3, 2, 3, false);
    graph2.addEdge(3, 5, 5, false);
    graph2.addEdge(3, 4, 4, false);
    graph2.addEdge(2, 5, 2, true);
    graph2.addEdge(4, 5, 5, false);
    graph2.addEdge(4, 7, 2, false);
    graph2.addEdge(7, 6, 1, false);
    graph2.addEdge(5, 6, 1, true);
    graph2.addEdge(1, 8, 3, true);
    graph2.addEdge(3, 9, 12, true);
    expectedPath = {1, 2, 5, 6};
    expectedWeight = 4;
    result = graph2.getShortestPath(1, 6, true);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );
    expectedPath = {4, 7, 6};
    expectedWeight = 3;
    result = graph2.getShortestPath(4, 6, true);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );
    expectedPath = {4, 7, 6, 5, 2};
    expectedWeight = 6;
    result = graph2.getShortestPath(4, 2, true);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph3;
    graph3.addEdge(1, 2, 1, false);
    graph3.addEdge(2, 3, 1, false);
    expectedPath = {};
    expectedWeight = -1;
    result = graph3.getShortestPath(3, 1, true);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph4;
    graph4.addEdge(1, 2, 3, false);
    graph4.addEdge(2, 3, 3, false);
    graph4.addEdge(3, 4, 3, false);
    graph4.addEdge(1, 5, 5, false);
    graph4.addEdge(5, 4, 5, false);
    expectedPath = {1, 2, 3, 4};
    expectedWeight = 9;
    result = graph4.getShortestPath(1, 4, true);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph5;
    graph5.addEdge(1, 2, 3, true);
    REQUIRE_THROWS_AS(graph5.getShortestPath(1, 3, true), std::logic_error);
}

TEST_CASE( "Simple contraction hierarchies test", "[BidirectionalSearch]") {
    Graph graph1;
    graph1.addEdge(1, 2, 5, true);
    graph1.addEdge(2, 3, 7, true);
    graph1.addEdge(1, 3, 1, true);
    HierarchyConstructor builder1(graph1);
    builder1.contractGraph();
    std::vector<uint64_t> expectedPath{1, 3};
    double expectedWeight = 1;
    auto result = graph1.getShortestPath(1, 3);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );


    Graph graph2;
    graph2.addEdge(1, 2, 5, true);
    graph2.addEdge(2, 3, 7, true);
    graph2.addEdge(1, 3, 1, true);
    graph2.addEdge(1, 4, 0.2, true);
    graph2.addEdge(4, 5, 0.3, true);
    graph2.addEdge(5, 3, 0.1, true);
    HierarchyConstructor builder2(graph2);
    builder2.contractGraph();
    expectedPath = {1, 4, 5, 3};
    expectedWeight = 0.6;
    result = graph2.getShortestPath(1, 3);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph3;
    graph3.addEdge(1, 2, 1, false);
    graph3.addEdge(1, 3, 2, true);
    graph3.addEdge(3, 2, 3, false);
    graph3.addEdge(3, 5, 5, false);
    graph3.addEdge(3, 4, 4, false);
    graph3.addEdge(2, 5, 2, true);
    graph3.addEdge(4, 5, 5, false);
    graph3.addEdge(4, 7, 2, false);
    graph3.addEdge(7, 6, 1, false);
    graph3.addEdge(5, 6, 1, true);
    graph3.addEdge(1, 8, 3, true);
    graph3.addEdge(3, 9, 12, true);
    HierarchyConstructor builder3(graph3);
    builder3.contractGraph();
    expectedPath = {1, 2, 5, 6};
    expectedWeight = 4;
    result = graph3.getShortestPath(1, 6);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );
    expectedPath = {4, 7, 6};
    expectedWeight = 3;
    result = graph3.getShortestPath(4, 6);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );
    expectedPath = {4, 7, 6, 5, 2};
    expectedWeight = 6;
    result = graph3.getShortestPath(4, 2);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph4;
    graph4.addEdge(1, 2, 1, false);
    graph4.addEdge(2, 3, 1, false);
    HierarchyConstructor builder4(graph4);
    builder4.contractGraph();
    expectedPath = {};
    expectedWeight = -1;
    result = graph4.getShortestPath(3, 1);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph5;
    graph5.addEdge(1, 2, 3, false);
    graph5.addEdge(2, 3, 3, false);
    graph5.addEdge(3, 4, 3, false);
    graph5.addEdge(1, 5, 5, false);
    graph5.addEdge(5, 4, 5, false);
    HierarchyConstructor builder5(graph5);
    builder5.contractGraph();
    expectedPath = {1, 2, 3, 4};
    expectedWeight = 9;
    result = graph5.getShortestPath(1, 4);
    REQUIRE( result.first == expectedPath );
    REQUIRE( result.second == expectedWeight );

    Graph graph6;
    graph6.addEdge(1, 2, 3, true);
    HierarchyConstructor builder6(graph6);
    builder6.contractGraph();
    REQUIRE_THROWS_AS(graph6.getShortestPath(1, 3), std::logic_error);
}

TEST_CASE( "Comprehensive contraction hierarchies test", "[BidirectionalSearch]") {

    const double EPSILON = 0.00001;
    const int NUM_TESTS = 25;
    int num_tests_conducted = 0;

    std::vector<Parser> parsers;
    parsers.emplace_back("test_input1.osm");
    parsers.emplace_back("test_input2.osm");
    parsers.emplace_back("test_input3.osm");

    // Random number generator for choosing start and end IDs.
    std::random_device rd;
    std::mt19937 engine(rd());

    for (auto& parser : parsers) {
        Graph contracted_graph = parser.constructRoadNetworkGraph().first;
        Graph graph = contracted_graph;
        auto vertices = graph.getVertices();
        HierarchyConstructor builder(contracted_graph);
        builder.contractGraph();

        std::vector<uint64_t> id_vector;
        id_vector.reserve(vertices.size());
        for(const auto& kv : vertices) {
            id_vector.push_back(kv.first);
        }

        std::uniform_int_distribution<int> dist(0, int(id_vector.size() -1));

        while (num_tests_conducted < NUM_TESTS) {
            // Getting a random start and end point in the graph.
            uint64_t start_id = id_vector[dist(engine)];
            uint64_t end_id = id_vector[dist(engine)];

            auto path1 = contracted_graph.getShortestPath(start_id, end_id);
            auto path2 = graph.getShortestPath(start_id, end_id, true);

            int i = 0;

            // Paths contain different number of nodes.
            REQUIRE(path1.first.size() == path2.first.size());
            REQUIRE(std::abs(path1.second - path2.second) < EPSILON);

            // Checks to see if paths contain different nodes.
            while (i < path1.first.size()) {
                REQUIRE(path1.first[i] == path2.first[i]);
                i++;
            }

            num_tests_conducted++;
        }
        num_tests_conducted = 0;
    }
}
