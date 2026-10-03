module;

#include <folly/hash/Hash.h>

module hasher;

std::size_t folly_hash(std::string_view value) noexcept
{
    return folly::hasher<std::string_view>{}(value);
}

