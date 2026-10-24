#include "Benchmark.hpp"
#include "FileReader.hpp"

import std;
import arg_parser;


static void printResults(const auto& results)
{
    std::println(
        "{:<100} {:>12} {:>12}",
        "Hash set",
        "Unique words",
        "Time (ms)"
    );

    std::println("{}", std::string(110, '-'));

    for (const auto& result : results)
    {
        std::println(
            "{:<100} {:>12} {:>12.3f}",
            result.setName,
            result.uniqueWords,
            result.milliseconds
        );
    }
}

int main(int argc, char* argv[])
{
    const auto parseResult = ArgParser::parse(argc, argv);

    if (not parseResult)
    {
        std::println("Argument parsing error: {}", parseResult.error());
        return 1;
    }

    const auto mmapResult = FileReader::mmap(parseResult->filename);
    if (not mmapResult)
    {
        std::println("Failed to memory-map file: {}", mmapResult.error().message());
        return 1;
    }

    const auto fileMemory = mmapResult->view;
    const auto benchmarkFn = parseResult->benchmark == "single"
                             ? &Benchmark::runOne
                             : &Benchmark::runCartesianProduct;

    auto benchmarkData = benchmarkFn(fileMemory);

    std::ranges::sort(benchmarkData, {}, &Benchmark::Result::milliseconds);

    printResults(benchmarkData);
}
