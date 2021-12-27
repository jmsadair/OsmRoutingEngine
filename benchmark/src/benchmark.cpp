#define ANKERL_NANOBENCH_IMPLEMENT
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <nanobench.h>
#include <random>
#include <fstream>
#include "Queue.h"

namespace {

    void gen(std::string const& typeName, char const* mustacheTemplate,
             ankerl::nanobench::Bench const& bench) {

        std::ofstream templateOut("mustache.template." + typeName);
        templateOut << mustacheTemplate;

        std::ofstream renderOut("mustache.render." + typeName);
        ankerl::nanobench::render(mustacheTemplate, bench, renderOut);
    }

}
TEST_CASE( "Queue pop benchmark", "[Queue]") {
    // Create a single benchmark instance that is used in multiple benchmark
    // runs, with different settings for complexityN.
    ankerl::nanobench::Bench bench;
    bench.title("Queue Push");

    // a RNG to generate input data
    ankerl::nanobench::Rng rng;

    Queue<uint64_t> queue;

    // Running the benchmark multiple times, with different number of elements
    for (auto queueSize :
            {10U, 20U, 50U, 100U, 200U, 500U, 1000U, 2000U, 5000U, 10000U}) {

        // fill up the set with random data
        while (queue.size() < queueSize) {
            queue.push(rng());
        }

        // Run the benchmark, provide setSize as the scaling variable.
        bench.complexityN(queue.size()).minEpochIterations(1000000).run("Queue pop", [&] {
            ankerl::nanobench::doNotOptimizeAway(queue.pop());
            queue.push(rng());
        });
    }

    gen("json", ankerl::nanobench::templates::json(), bench);
    gen("html", ankerl::nanobench::templates::htmlBoxplot(), bench);
    gen("csv", ankerl::nanobench::templates::csv(), bench);

    // calculate BigO complexy best fit and print the results
    std::cout << bench.complexityBigO() << std::endl;
}