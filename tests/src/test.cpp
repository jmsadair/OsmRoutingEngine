#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "queue.h"

TEST_CASE( "Tests priority queue pop function", "[queue]") {
    queue Q;
    Q.push(1, 1);
    Q.push(2, 2);
    Q.push(3, 3);
    REQUIRE( Q.pop() == 1 );
    REQUIRE( Q.pop() == 2 );
    REQUIRE( Q.pop() == 3 );
}