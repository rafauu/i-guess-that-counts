module;

#include <xxhash.h>
#include <absl/hash/hash.h>
#include <ankerl/unordered_dense.h>
#include <wyhash.h>

export module hasher;

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

std::size_t folly_hash(std::string_view value) noexcept;

export struct FollyHasher
{
    using is_avalanching = void;

    //template <typename T>
    //[[nodiscard]] static std::size_t operator()(const T& obj) noexcept
    [[nodiscard]] static std::size_t operator()(std::string_view obj) noexcept
    {
        return folly_hash(obj);
    }
};

export struct StdHasher
{
    using is_avalanching = void;

    template <typename T>
    [[nodiscard]] static std::size_t operator()(const T& obj) noexcept(noexcept(std::hash<T>{}(obj)))
    {
        return std::hash<T>{}(obj);
    }
};

export struct AbslHasher
{
    using is_avalanching = void;

    template <typename T>
    [[nodiscard]] static std::size_t operator()(const T& key) noexcept
    {
        return absl::Hash<T>{}(key);
    }
};

export struct AnkerlHasher
{
    using is_avalanching = void;

    template <typename T>
    [[nodiscard]] static std::size_t operator()(const T& key) noexcept
    {
        return ankerl::unordered_dense::hash<T>{}(key);
    }
};

[[nodiscard]] std::size_t wyhash_bytes(const void* data, std::size_t size) noexcept
{
    return wyhash(data, size, 0, _wyp);
}

export struct WyHasher
{
    using is_avalanching = void;

    template <typename T>
    [[nodiscard]] static std::size_t operator()(const T& key) noexcept
    {
        if constexpr (std::is_integral_v<T> ||
                      std::is_enum_v<T> ||
                      std::is_pointer_v<T>)
        {
            return wyhash_bytes(&key, sizeof(T));
        }
        else if constexpr (std::is_same_v<T, std::string> ||
                           std::is_same_v<T, std::string_view>)
        {
            return wyhash_bytes(key.data(), key.size());
        }
        else
        {
            return wyhash_bytes(&key, sizeof(T));
        }
    }
};
