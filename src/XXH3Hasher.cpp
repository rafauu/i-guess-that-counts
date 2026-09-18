#define XXH_INLINE_ALL
#include <xxhash.h>

#include "XXH3Hasher.hpp"

size_t XXH3Hasher::operator()(std::string_view sv) noexcept
{
    return static_cast<size_t>(XXH3_64bits(sv.data(), sv.size()));
}
