#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "AffinityHandler.hpp"
#include "UniqueWordsCounter.hpp"

static constexpr auto isWhitespaceWithoutLocale = [] (char c) noexcept {
    return static_cast<unsigned char>(c) <= 32;
};

size_t UniqueWordsCounter::count(std::string_view memory)
{
    if (memory.empty())
        return 0;

    // 1. Boundary Chunking
    std::vector<std::string_view> chunks;
    chunks.reserve(memory.size() / TARGET_CHUNK_SIZE + 1);

    size_t offset = 0;
    while (offset < memory.size())
    {
        size_t end = std::min(offset + TARGET_CHUNK_SIZE, memory.size());
        if (end < memory.size())
        {
            while (end > offset && not isWhitespaceWithoutLocale(static_cast<unsigned char>(memory[end - 1])))
                --end;
            if (end == offset) end = std::min(offset + TARGET_CHUNK_SIZE, memory.size());
        }
        chunks.emplace_back(memory.data() + offset, end - offset);
        offset = end;
    }

    const unsigned numThreads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<ThreadResult> threadResults(numThreads);
    std::atomic<size_t> nextChunkIndex{0};

    // 2. MAP Phase
    {
        std::vector<std::jthread> workers;
        workers.reserve(numThreads);

        for (auto t : std::views::iota(0u, numThreads))
        {
            workers.emplace_back([&, t] {
                AffinityHandler::pinThreadToHwCore(t);

                HashSet<std::string_view> localStackSet;
                localStackSet.reserve(65536);

                while (true)
                {
                    size_t idx = nextChunkIndex.fetch_add(1, std::memory_order_relaxed);
                    if (idx >= chunks.size())
                        break;

                    const auto& chunk = chunks[idx];
                    auto it = std::ranges::begin(chunk);
                    auto end = std::ranges::end(chunk);

                    while (it != end)
                    {
                        it = std::ranges::find_if_not(it, end, isWhitespaceWithoutLocale);
                        if (it == end)
                            break;

                        auto start = it;
                        it = std::ranges::find_if(start, end, isWhitespaceWithoutLocale);

                        if (start != it)
                        {
                            localStackSet.emplace(std::string_view{start, it});
                        }
                    }

                }

                threadResults[t].wordSet = std::move(localStackSet);
            });
        }
    }

    // 3. Parallel Tree Reduction Phase (Merging thread sets)
    size_t activeCount = numThreads;
    while (activeCount > 1)
    {
        size_t half = (activeCount + 1) / 2;
        {
            std::vector<std::jthread> mergeWorkers;
            mergeWorkers.reserve(activeCount / 2);

            for (auto i : std::views::iota(0uz, activeCount / 2uz))
            {
                size_t sourceIdx = i + half;
                if (sourceIdx < activeCount)
                {
                    mergeWorkers.emplace_back([&, targetIdx = i, sourceIdx] {
                        AffinityHandler::pinThreadToHwCore(targetIdx);

                        auto& targetSet = threadResults[targetIdx].wordSet;
                        auto& sourceSet = threadResults[sourceIdx].wordSet;

                        targetSet.reserve(targetSet.size() + sourceSet.size());

                        targetSet.insert(
                            std::make_move_iterator(sourceSet.begin()),
                            std::make_move_iterator(sourceSet.end())
                        );
                    });
                }
            }
        }
        activeCount = half;
    }

    return threadResults[0].wordSet.size();
}
