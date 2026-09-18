#include <print>
#include <mio/mmap.hpp>
#include "SystemUtils.hpp"
#include "UniqueWordsCounter.hpp"


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::print("Please specify one input file\n");
        return 1;
    }

    const auto filename = argv[1];
    std::print("File to be mapped: {}\n", filename);

    std::error_code ec;
    auto ro_mmap = mio::make_mmap_source(filename, ec);
    if (ec)
    {
        std::print("Failed to memory-map file: {}\n", ec.message());
        return 1;
    }

    std::string_view fileMemory{ro_mmap.data(), ro_mmap.size()};

    SystemUtils::adviseSequentialAccess(fileMemory);

    std::print("Size of mapped memory: {}\n", fileMemory.size());

    UniqueWordsCounter counter{};
    auto result = counter.count(fileMemory);
    std::print("Unique words in file: {}\n", result);

    return 0;
}
