#include "FileReader.hpp"
#include <print>

import system_utils;

namespace FileReader
{
    std::expected<MmapResult, std::error_code> mmap(const char* filename)
    {
        std::println("File to be mapped: {}", filename);

        std::error_code ec;
        auto mmapSource = mio::make_mmap_source(filename, ec);

        if (ec)
        {
            return std::unexpected(ec);
        }

        std::string_view fileMemory{mmapSource.data(), mmapSource.size()};

        std::println("Size of mapped memory: {}", fileMemory.size());

        SystemUtils::adviseHugePage(fileMemory);

        return MmapResult{
            .keepAlive = std::move(mmapSource),
            .fileMemory = fileMemory
        };
    }
}
