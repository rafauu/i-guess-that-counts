#include "Benchmark.hpp"

#include <nameof.hpp>
#include <parallel_hashmap/phmap.h>
#include <tsl/hopscotch_set.h>
#include <tsl/bhopscotch_set.h>
#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/unordered/unordered_node_set.hpp>
#include <boost/unordered/unordered_set.hpp>
#include <folly/container/F14Set.h>
#include <robin_hood.h>
#include <tsl/robin_set.h>
#include <tsl/sparse_set.h>
#include <ankerl/unordered_dense.h>
#include <absl/container/flat_hash_set.h>

import std;
import hasher;
import unique_words_counter;

namespace Benchmark
{

template <typename... Ts>
struct TypeList {};

template <template <typename...> class Container>
struct SetTag
{
    template <typename Key, typename Hasher>
    using type = Container<Key, Hasher>;
};

// Some libraries use template signatures with non-type template parameters
// or multiple mandatory type parameters that block direct template passing
// without default arguments.

template <typename Key, typename Hash>
using RobinHoodSet = robin_hood::unordered_flat_set<Key, Hash>;

template <typename Key, typename Hash>
using TslRobinSet = tsl::robin_set<Key, Hash>;

template <typename Key, typename Hash>
using TslSparseSet = tsl::sparse_set<Key, Hash>;

template <typename Key, typename Hash>
using PhMapParallelFlatHashSet = phmap::parallel_flat_hash_set<Key, Hash>;

template <typename Key, typename Hash>
using PhMapParallelNodeHashSet = phmap::parallel_node_hash_set<Key, Hash>;

template <typename Key, typename Hash>
using TslHopScotchSet = tsl::hopscotch_set<Key, Hash>;

template <typename Key, typename Hash>
using TslBHopScotchSet = tsl::bhopscotch_set<Key, Hash>;

using HashSets = TypeList<
    SetTag<std::unordered_set>,
    SetTag<boost::unordered_set>,
    SetTag<PhMapParallelNodeHashSet>,
    SetTag<phmap::node_hash_set>,
    SetTag<boost::unordered_node_set>,
    SetTag<TslHopScotchSet>,
    SetTag<TslBHopScotchSet>,
    SetTag<TslSparseSet>,
    SetTag<TslRobinSet>,
    SetTag<RobinHoodSet>,
    SetTag<PhMapParallelFlatHashSet>,
    SetTag<ankerl::unordered_dense::set>,
    SetTag<folly::F14FastSet>,
    SetTag<absl::flat_hash_set>,
    SetTag<phmap::flat_hash_set>,
    SetTag<boost::unordered_flat_set>
>;

using Hashers = TypeList<
    FollyHasher,
    StdHasher,
    FarmHasher,
    WyHasher,
    RapidHasher,
    XXH3Hasher,
    AnkerlHasher,
    AbslHasher
>;

template <
    template <class, class...> class Set,
    typename Hasher
>
Result runImpl(std::string_view fileMemory)
{
    using Clock = std::chrono::steady_clock;

    UniqueWordsCounter<Set, Hasher> counter{};

    const auto start = Clock::now();
    const auto count = counter.count(fileMemory);
    const auto stop = Clock::now();

    return {
        .setName = nameof::nameof_type<Set<void*, Hasher>>(),
        .uniqueWords = count,
        .milliseconds = std::chrono::duration<double, std::milli>(stop - start).count()
    };
}

template <typename... Ts, typename F>
constexpr void forEachType(TypeList<Ts...>, F&& f)
{
    (f.template operator()<Ts>(), ...);
}

std::vector<Result> runCartesianProduct(std::string_view fileMemory)
{
    std::vector<Result> results;

    forEachType(HashSets{}, [&]<typename SetTagType> {
        forEachType(Hashers{}, [&]<typename HasherType> {
            results.push_back(
                runImpl<SetTagType::template type, HasherType>(fileMemory)
            );
        });
    });

    return results;
}

Result runOne(std::string_view fileMemory)
{
    return runImpl<boost::unordered_flat_set, AbslHasher>(fileMemory);
}

}
