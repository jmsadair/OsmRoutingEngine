#define CATCH_CONFIG_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#include <catch2/catch.hpp>
#include <nanobench.h>
#include <random>
#include <fstream>
#include <memory>
#include "Queue.h"
#include "HierarchyConstructor.h"
#include "Serialize.h"
#include "OsmParser.h"

namespace {
    void gen(std::string const& typeName, char const* mustacheTemplate, ankerl::nanobench::Bench const& bench) {
        std::ofstream templateOut("mustache.template." + typeName);
        templateOut << mustacheTemplate;
        std::ofstream renderOut("mustache.render." + typeName);
        ankerl::nanobench::render(mustacheTemplate, bench, renderOut);
    }

    // Generates a vector of vertex IDs that will be chosen from when selecting random sources and targets.
    std::vector<uint64_t> generateIdVector(Graph* graph) {
        // Generate a vector of all the vertex IDs in the graph.
        auto vertices = graph->getVertices();
        std::vector<uint64_t> id_vector;
        id_vector.reserve(vertices.size());
        for(const auto& kv : vertices) {
            id_vector.push_back(kv.first);
        }
        return id_vector;
    }

    // Benchmarks how long it takes to find a route.
    void searchBench(ankerl::nanobench::Bench* bench, char const* name, Graph* graph, bool standard) {
        std::vector<uint64_t> id_vector = generateIdVector(graph);
        ankerl::nanobench::Rng rng;
        bench->minEpochIterations(2000).run(name, [&]() {
            graph->getShortestPath(id_vector[rng.bounded(id_vector.size())], id_vector[rng.bounded(id_vector.size())], standard);
        });
    }
}

TEST_CASE("Queue::MinHeap pop & push benchmark", "[MinHeap]") {
    ankerl::nanobench::Bench bench;
    bench.title("Random pop & push in MinHeap");
    ankerl::nanobench::Rng rng;
    Queue::MinHeap<uint64_t> queue;
    uint64_t const bitmask = UINT64_C(0xff);
    for (auto queueSize :
            {100U, 1000U, 10000U, 100000U, 1000000U}) {
        while (queue.size() < queueSize) {
            queue.push(rng());
        }
        bench.minEpochIterations(1000000).run("Queue::MinHeap pop & push", [&] {
            queue.push(rng() & bitmask);
            queue.pop();
        }).doNotOptimizeAway(&queue);
    }
}

TEST_CASE( "Queue::MinHeap pop and push benchmark time complexity", "[MinHeap]") {
    ankerl::nanobench::Bench bench;
    bench.title("Random pop & push in MinHeap complexity");
    ankerl::nanobench::Rng rng;
    Queue::MinHeap<uint64_t> queue;
    for (auto queueSize :
            {100U, 1000U, 10000U, 100000U, 1000000U}) {
        while (queue.size() < queueSize) {
            queue.push(rng());
        }
        bench.complexityN(queue.size()).minEpochIterations(10000000).run("Queue::MinHeap pop & push complexity", [&] {
            queue.pop();
            queue.push(rng());
        }).doNotOptimizeAway(&queue);
    }
    std::cout << bench.complexityBigO() << std::endl;
}

TEST_CASE("Bidirectional search on city of Denver", "[BidirectionalSearch]") {
    ankerl::nanobench::Bench bench;
    bench.title("Bidirectional search on city of Denver");
    bench.timeUnit(std::chrono::milliseconds(1), "ms");
    Graph graph = *std::make_unique<Graph>(Serialize::load<Graph>("denver_graph_contracted.bin"));
    searchBench(&bench, "Bidirectional Search", &graph, false);
}

TEST_CASE("Bidirectional search on state of Massachusetts benchmark", "[BidirectionalSearch]") {
    ankerl::nanobench::Bench bench;
    bench.title("Bidirectional search on state of Massachusetts");
    bench.timeUnit(std::chrono::milliseconds(1), "ms");
    Graph graph = *std::make_unique<Graph>(Serialize::load<Graph>("massachusetts_graph_contracted.bin"));
    searchBench(&bench, "Bidirectional Search", &graph, false);
}

TEST_CASE("Bidirectional search on US NorthEast benchmark", "[BidirectionalSearch]") {
    ankerl::nanobench::Bench bench;
    bench.title("Bidirectional search on US NorthEast");
    bench.timeUnit(std::chrono::milliseconds(1), "ms");
    Graph graph = *std::make_unique<Graph>(Serialize::load<Graph>("us_northeast_graph_contracted.bin"));
    searchBench(&bench, "Bidirectional Search", &graph, false);
}

TEST_CASE("Bidirectional search on US South benchmark", "[BidirectionalSearch]") {
    ankerl::nanobench::Bench bench;
    bench.title("Bidirectional search on US South");
    bench.timeUnit(std::chrono::milliseconds(1), "ms");
    Graph graph = *std::make_unique<Graph>(Serialize::load<Graph>("us_south_graph_contracted.bin"));
    searchBench(&bench, "Bidirectional Search", &graph, false);
}

TEST_CASE("Bidirectional search on various graph sizes", "[BidirectionalSearch]") {
    ankerl::nanobench::Bench bench;
    bench.title("Bidirectional search on various graph sizes");
    bench.timeUnit(std::chrono::milliseconds(1), "ms");
    std::vector<std::pair<const char*, std::string>> files{std::make_pair("denver_graph_contracted.bin", "City of Denver"), std::make_pair("massachusetts_graph_contracted.bin", "State of Massachusetts"),
                                                                std::make_pair("us_northeast_graph_contracted.bin", "US NorthEast"), std::make_pair("us_south_graph_contracted.bin", "US South")};

    for (const auto& [filename, title] : files) {
        auto contracted_graph_ptr = std::make_unique<Graph>(Serialize::load<Graph>(filename));
        Graph contracted_graph = *contracted_graph_ptr;

        // Generate a vector of all the vertex IDs in the graph.
        std::vector<uint64_t> id_vector = generateIdVector(&contracted_graph);

        // An RNG to generate random source and target IDs.
        ankerl::nanobench::Rng rng;
        rng.shuffle(id_vector);

        std::string bench_iter_name = title + " (" + std::to_string(contracted_graph.getNumVertices()) + " vertices)";

        bench.minEpochIterations(15000).run(bench_iter_name, [&] {
            contracted_graph.getShortestPath(id_vector[rng.bounded(id_vector.size())],
                                             id_vector[rng.bounded(id_vector.size())], false);
        }).doNotOptimizeAway(&contracted_graph);
    }

    gen("json", ankerl::nanobench::templates::json(), bench);
    gen("html", ankerl::nanobench::templates::htmlBoxplot(), bench);
    gen("csv", ankerl::nanobench::templates::csv(), bench);
}
