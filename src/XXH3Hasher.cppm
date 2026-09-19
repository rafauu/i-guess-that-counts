module;

#include <xxhash.h>

export module xxh3_hasher;

import std;

export struct XXH3Hasher
{
    // Informs ankerl::unordered_dense that XXH3 produces high-entropy,
    // well-distributed bits. This disables ankerl's default secondary mixing
    // step (wyhash bit-avalanching), saving multiple CPU instructions per lookup.
    using is_avalanching = void;

    [[nodiscard]] static std::size_t operator()(std::string_view sv) noexcept
    {
        return static_cast<std::size_t>(XXH3_64bits(sv.data(), sv.size()));
    }
};
