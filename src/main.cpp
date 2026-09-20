#include <ankerl/unordered_dense.h>
#include <mio/mmap.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <absl/container/flat_hash_set.h>
#include <absl/hash/hash.h>
#include <wyhash.h>

import std;

import system_utils;
import unique_words_counter;
import xxh3_hasher;

struct StdHasher {
    template <typename T>
    std::size_t operator()(const T& obj) const noexcept(noexcept(std::hash<T>{}(obj))) {
        return std::hash<T>{}(obj);
    }
};

struct AbslHasher {
    template <typename T>
    std::size_t operator()(const T& key) const noexcept {
        return absl::Hash<T>{}(key);
    }
};

struct AnkerlHasher {
    template <typename T>
    std::size_t operator()(const T& key) const noexcept {
        return ankerl::unordered_dense::hash<T>{}(key);
    }
};

struct WyHasher {
    template <typename T>
    std::size_t operator()(const T& key) const noexcept {
        static constexpr uint64_t seed = 0;

        if constexpr (std::is_integral_v<T> || std::is_enum_v<T> || std::is_pointer_v<T>) {
            return wyhash(&key, sizeof(T), seed, _wyp);
        }
        else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
            return wyhash(key.data(), key.size(), seed, _wyp);
        }
        else {
            return wyhash(&key, sizeof(T), seed, _wyp);
        }
    }
};


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

    //UniqueWordsCounter<ankerl::unordered_dense::set, XXH3Hasher> counter{};
    //UniqueWordsCounter<ankerl::unordered_dense::set, StdHasher> counter{};
    //UniqueWordsCounter<std::unordered_set, StdHasher> counter{};
    //UniqueWordsCounter<boost::unordered_flat_set, StdHasher> counter{};
    //UniqueWordsCounter<boost::unordered_flat_set, XXH3Hasher> counter{};
    //UniqueWordsCounter<absl::flat_hash_set, XXH3Hasher> counter{};
    //UniqueWordsCounter<boost::unordered_flat_set, AbslHasher> counter{};
    //UniqueWordsCounter<boost::unordered_flat_set, AnkerlHasher> counter{};
    //UniqueWordsCounter<ankerl::unordered_dense::set, AnkerlHasher> counter{};
    //UniqueWordsCounter<ankerl::unordered_dense::set, WyHasher> counter{};
    UniqueWordsCounter<boost::unordered_flat_set, WyHasher> counter{};
    auto result = counter.count(fileMemory);
    std::print("Unique words in file: {}\n", result);

    return 0;
}
