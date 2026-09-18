#pragma once

#include <cstddef>
#include <new>
#include <string_view>

#include <ankerl/unordered_dense.h>

#include "XXH3Hasher.hpp"

constexpr auto operator""_MB(unsigned long long value) noexcept
{
    return value * 1024 * 1024;
}

class UniqueWordsCounter
{
private:
    static constexpr size_t TARGET_CHUNK_SIZE = 2_MB;

    template <typename T>
    using HashSet = ankerl::unordered_dense::set<T, XXH3Hasher>;

    struct alignas(std::hardware_destructive_interference_size) ThreadResult
    {
        HashSet<std::string_view> wordSet;
    };

public:
    size_t count(std::string_view memory);
};
