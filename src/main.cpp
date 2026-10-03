#include <mio/mmap.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/unordered/unordered_node_set.hpp>
#include <boost/unordered/unordered_set.hpp>
#include <folly/container/F14Set.h>
#include <robin_hood.h>
#include <tsl/robin_set.h>
#include <tsl/sparse_set.h>
#include <ankerl/unordered_dense.h>
#include <absl/container/flat_hash_set.h>

//import std; GCC15 ICE
#include <print>
#include <system_error>
#include <string_view>

import system_utils;
import unique_words_counter;
import hasher;

template <typename Key, typename Hash>
using RobinHoodSet = robin_hood::unordered_flat_set<Key, Hash>;

template <typename Key, typename Hash>
using TslRobinSet = tsl::robin_set<Key, Hash>;

template <typename Key, typename Hash>
using TslSparseSet = tsl::sparse_set<Key, Hash>;

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

    //UniqueWordsCounter<ankerl::unordered_dense::set, XXH3Hasher> counter{}; //115ms
    //UniqueWordsCounter<ankerl::unordered_dense::set, StdHasher> counter{}; //135ms
    //UniqueWordsCounter<std::unordered_set, StdHasher> counter{}; //255ms
    //UniqueWordsCounter<boost::unordered_flat_set, StdHasher> counter{}; //127ms
    UniqueWordsCounter<boost::unordered_flat_set, XXH3Hasher> counter{}; //108ms //105ms
    //UniqueWordsCounter<absl::flat_hash_set, XXH3Hasher> counter{}; //109ms //106ms
    //UniqueWordsCounter<absl::flat_hash_set, WyHasher> counter{}; //NA //111ms
    //UniqueWordsCounter<absl::flat_hash_set, AbslHasher> counter{}; //NA //107ms
    //UniqueWordsCounter<boost::unordered_flat_set, AbslHasher> counter{}; //107ms //104ms
    //UniqueWordsCounter<boost::unordered_node_set, AbslHasher> counter{}; //NA //138ms
    //UniqueWordsCounter<boost::unordered_set, AbslHasher> counter{}; //NA //184ms
    //UniqueWordsCounter<boost::unordered_flat_set, AnkerlHasher> counter{}; //111ms
    //UniqueWordsCounter<ankerl::unordered_dense::set, AnkerlHasher> counter{}; //116ms
    //UniqueWordsCounter<ankerl::unordered_dense::set, WyHasher> counter{}; //121ms
    //UniqueWordsCounter<boost::unordered_flat_set, WyHasher> counter{}; //111ms
    //UniqueWordsCounter<boost::unordered_flat_set, FollyHasher> counter{}; //141ms
    //UniqueWordsCounter<folly::F14FastSet, WyHasher> counter{}; //120ms
    //UniqueWordsCounter<RobinHoodSet, WyHasher> counter{}; //121ms
    //UniqueWordsCounter<TslRobinSet, WyHasher> counter{}; //139ms
    //UniqueWordsCounter<TslSparseSet, WyHasher> counter{}; //131ms

    auto result = counter.count(fileMemory);
    std::print("Unique words in file: {}\n", result);

    return 0;
}
