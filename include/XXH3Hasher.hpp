#pragma once

#include <string_view>

struct XXH3Hasher
{
    // Informs ankerl::unordered_dense that XXH3 produces high-entropy,
    // well-distributed bits. This disables ankerl's default secondary mixing
    // step (wyhash bit-avalanching), saving multiple CPU instructions per lookup.
    using is_avalanching = void;

    [[nodiscard]] static size_t operator()(std::string_view sv) noexcept;
};
