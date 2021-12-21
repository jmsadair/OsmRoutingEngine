#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Queue.h"
#include "Graph.h"
#include "OsmParser.h"
#include <stdexcept>

TEST_CASE( "Tests priority queue pop/push functions", "[Queue]") {
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

TEST_CASE( "Tests priority queue empty function", "[Queue]") {
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

TEST_CASE( "Tests priority queue size function", "[Queue]") {
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

TEST_CASE( "Tests priority queue peek function", "[Queue]") {
    Queue<int> Q1;
    Q1.push(1);
    Q1.push(2);
    Q1.push(3);
    REQUIRE( *Q1.peek() == 1 );
    Q1.pop();
    REQUIRE( *Q1.peek() == 2 );
    Q1.pop();
    REQUIRE( *Q1.peek() == 3 );
}

TEST_CASE( "Tests graph addEdge, edgeExists, and removeEdge functions", "[Graph]") {
    Graph graph1;
    graph1.add_edge(1, 4, 5, true);
    graph1.add_edge(5, 9, 10, false);
    REQUIRE( graph1.edge_exists(1, 4) == true );
    REQUIRE( graph1.edge_exists(4, 1) == true );
    REQUIRE( graph1.edge_exists(5, 9) == true );
    REQUIRE( graph1.edge_exists(9, 5) == false );
    graph1.remove_edge(1, 4);
    REQUIRE( graph1.edge_exists(1, 4) == false );
    REQUIRE( graph1.edge_exists(4, 1) == true );
    REQUIRE( graph1.edge_exists(5, 9) == true );
    REQUIRE( graph1.edge_exists(9, 5) == false );
    graph1.remove_edge(4, 1);
    REQUIRE( graph1.edge_exists(1, 4) == false );
    REQUIRE( graph1.edge_exists(4, 1) == false );
    REQUIRE( graph1.edge_exists(5, 9) == true );
    REQUIRE( graph1.edge_exists(9, 5) == false );

    Graph graph2;
    graph2.add_edge(1, 2, 4, true);
    graph2.add_edge(3, 4, 3, true);
    graph2.add_edge(5, 6, 7, true);
    graph2.add_edge(7, 8, 12, true);
    graph2.add_edge(9, 10, 33, false);
    graph2.add_edge(11, 12, 1, false);
    graph2.add_edge(1, 12, 132, false);
    REQUIRE( graph2.edge_exists(1, 2) == true );
    REQUIRE( graph2.edge_exists(2, 1) == true );
    REQUIRE( graph2.edge_exists(3, 4) == true );
    REQUIRE( graph2.edge_exists(4, 3) == true );
    REQUIRE( graph2.edge_exists(5, 6) == true );
    REQUIRE( graph2.edge_exists(6, 5) == true );
    REQUIRE( graph2.edge_exists(7, 8) == true );
    REQUIRE( graph2.edge_exists(8, 7) == true );
    REQUIRE( graph2.edge_exists(9, 10) == true );
    REQUIRE( graph2.edge_exists(10, 9) == false );
    REQUIRE( graph2.edge_exists(11, 12) == true );
    REQUIRE( graph2.edge_exists(12, 11) == false );
    REQUIRE( graph2.edge_exists(1, 12) == true );
    graph2.remove_edge(2, 1);
    graph2.remove_edge(1, 12);
    graph2.remove_edge(7, 8);
    graph2.remove_edge(4, 3);
    REQUIRE( graph2.edge_exists(1, 2) == true );
    REQUIRE( graph2.edge_exists(2, 1) == false );
    REQUIRE( graph2.edge_exists(3, 4) == true );
    REQUIRE( graph2.edge_exists(4, 3) == false );
    REQUIRE( graph2.edge_exists(5, 6) == true );
    REQUIRE( graph2.edge_exists(6, 5) == true );
    REQUIRE( graph2.edge_exists(7, 8) == false );
    REQUIRE( graph2.edge_exists(8, 7) == true );
    REQUIRE( graph2.edge_exists(9, 10) == true );
    REQUIRE( graph2.edge_exists(10, 9) == false );
    REQUIRE( graph2.edge_exists(11, 12) == true );
    REQUIRE( graph2.edge_exists(12, 11) == false );
    REQUIRE( graph2.edge_exists(1, 12) == false );

    Graph graph3;
    graph3.add_edge(1, 2, 3, false);
    REQUIRE_THROWS_AS( graph3.remove_edge(2, 1), std::logic_error );
}

TEST_CASE( "Tests graph getNumVertices and getNumEdges functions", "[Graph]") {
    Graph graph1;
    graph1.add_edge(1, 2, 5, true);
    REQUIRE( graph1.get_num_edges() == 2 );
    REQUIRE( graph1.get_num_vertices() == 2 );
    graph1.add_edge(2, 3, 5, false);
    REQUIRE( graph1.get_num_edges() == 3 );
    REQUIRE( graph1.get_num_vertices() == 3 );

    Graph graph2;
    graph2.add_edge(1, 2, 3, true);
    REQUIRE( graph2.get_num_edges() == 2 );
    REQUIRE( graph2.get_num_vertices() == 2);
    graph2.remove_edge(2, 1);
    REQUIRE( graph2.get_num_edges() == 1 );
    REQUIRE( graph2.get_num_vertices() == 1 ); // Fix me: 1
}