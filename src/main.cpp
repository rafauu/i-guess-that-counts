#include <system_error>
#include <fmt/core.h>
#include <mio/mmap.hpp>
#include <span>
#include "UniqueWordsCounter.hpp"


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fmt::print("Please specify one input file\n");
        return -1;
    }

    try
    {
        const auto filename = argv[1];

        fmt::print("File to be mapped: {}\n", filename);
        mio::mmap_source memory{filename};

        const unsigned long long memorySize = memory.size();
        fmt::print("Size of mapped memory: {}\n", memorySize);

        UniqueWordsCounter counter;
        auto result = counter.count(std::span{memory.data(), memorySize});
        fmt::print("Unique words in file: {}\n", result);
    }
    catch (const std::system_error& e)
    {
        fmt::print("Exception catched: {}\n", e.what());
        return -1;
    }

    return 0;
}
