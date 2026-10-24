#include "Benchmark.hpp"
#include "FileReader.hpp"

import std;

//import compact_flat_set;
//import file_mapper;


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::println("Please specify one input file");
        return 1;
    }

    auto mmapResult = FileReader::mmap(argv[1]);
    if (not mmapResult)
    {
        std::println("Failed to memory-map file: {}", mmapResult.error().message());
        return 1;
    }

    const auto fileMemory = mmapResult.value().fileMemory;

    auto _ = Benchmark::runOne(fileMemory);
    auto results = Benchmark::runCartesianProduct(fileMemory);

    std::ranges::sort(results, {}, &Benchmark::Result::milliseconds);

    std::println(
        "{:<90} {:>12} {:>12}",
        "Hash set",
        "Unique words",
        "Time (ms)"
    );

    std::println("{}", std::string(110, '-'));

    for (const auto& result : results)
    {
        std::println(
            "{:<90} {:>12} {:>12.3f}",
            result.setName,
            result.uniqueWords,
            result.milliseconds
        );
    }
}
