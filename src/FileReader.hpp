#pragma once

#include <system_error>
#include <string_view>
#include <expected>
#include <mio/mmap.hpp>

namespace FileReader
{
    struct MmapResult
    {
        mio::mmap_source keepAlive;
        std::string_view fileMemory;
    };

    std::expected<MmapResult, std::error_code> mmap(const char* filename);
}
