module;

export module unique_words_counter;

import system_utils;
import text_processor;

import std;


consteval std::size_t operator""_MB(unsigned long long value) noexcept
{
    return static_cast<std::size_t>(value) * 1024 * 1024;
}

export template <
    template <typename, typename...> class Container,
    typename Hasher
>
class UniqueWordsCounter
{
private:
    static constexpr std::size_t TARGET_CHUNK_SIZE = 2_MB;
    static constexpr std::size_t NUM_PARTITIONS = 8;
    static constexpr std::size_t INITIAL_CAPACITY_PER_WORKER_PARTITION = 4096;

    template <typename T>
    using HashSet = Container<T, Hasher>;

    struct alignas(std::hardware_destructive_interference_size) ThreadResult
    {
        std::array<HashSet<std::string_view>, NUM_PARTITIONS> partitions;
    };

public:
    std::size_t count(std::string_view memory)
    {
        if (memory.empty())
        {
            return 0;
        }

        // 1. Boundary Chunking
        [[maybe_unused]] auto t0 = std::chrono::high_resolution_clock::now();
        std::vector<std::string_view> chunks;
        chunks.reserve(memory.size() / TARGET_CHUNK_SIZE + 1);

        std::size_t offset = 0;
        while (offset < memory.size())
        {
            std::size_t end = std::min(offset + TARGET_CHUNK_SIZE, memory.size());
            if (end < memory.size())
            {
                while (end > offset and not TextProcessor::isWhitespace(static_cast<unsigned char>(memory[end - 1])))
                {
                    --end;
                }
                if (end == offset)
                {
                    end = std::min(offset + TARGET_CHUNK_SIZE, memory.size());
                }
            }
            chunks.emplace_back(memory.data() + offset, end - offset);
            offset = end;
        }

        const unsigned numThreads = std::max(1u, std::thread::hardware_concurrency());

        std::vector<ThreadResult> threadResults(numThreads);

        for (auto& result : threadResults)
        {
            for (auto& partition : result.partitions)
            {
                partition.reserve(INITIAL_CAPACITY_PER_WORKER_PARTITION);
            }
        }

        std::atomic<std::size_t> nextChunkIndex{0};

        [[maybe_unused]] auto t1 = std::chrono::high_resolution_clock::now();

        Hasher hasher{};

        // 2. MAP Phase (Tokenize & Direct Hash Partitioning)
        {
            std::vector<std::jthread> workers;
            workers.reserve(numThreads);

            for (auto t : std::views::iota(0u, numThreads))
            {
                workers.emplace_back([&, t] {
                    SystemUtils::Affinity::pinThreadToHwCore(t);

                    while (true)
                    {
                        std::size_t idx = nextChunkIndex.fetch_add(1, std::memory_order_relaxed);
                        if (idx >= chunks.size())
                        {
                            break;
                        }

                        const auto& chunk = chunks[idx];
                        const char* ptr = chunk.data();
                        const char* end = chunk.data() + chunk.size();

                        while (ptr < end)
                        {
                            const auto wordRange = TextProcessor::Simd::nextWord(ptr, end);
                            if (wordRange.begin == end)
                            {
                                break;
                            }

                            const auto length = static_cast<std::size_t>(wordRange.end - wordRange.begin);
                            const std::string_view word{wordRange.begin, length};

                            const auto hashedWord = hasher(word);
                            const auto partitionIdx = hashedWord & (NUM_PARTITIONS - 1);

                            threadResults[t].partitions[partitionIdx].emplace(word);

                            ptr = wordRange.end;
                        }
                    }
                });
            }
        }

        // 3. REDUCE Phase (Direct Single-Pass Partition Reduction)
        [[maybe_unused]] auto t2 = std::chrono::high_resolution_clock::now();
        {
            std::vector<std::jthread> mergeWorkers;
            mergeWorkers.reserve(NUM_PARTITIONS);

            for (auto p : std::views::iota(0uz, NUM_PARTITIONS))
            {
                mergeWorkers.emplace_back([&, p] {
                    SystemUtils::Affinity::pinThreadToHwCore(static_cast<unsigned>(p % numThreads));

                    auto& targetPartition = threadResults[0].partitions[p];

                    for (std::size_t t = 1; t < numThreads; ++t)
                    {
                        auto& sourcePartition = threadResults[t].partitions[p];
                        targetPartition.insert(
                            std::make_move_iterator(sourcePartition.begin()),
                            std::make_move_iterator(sourcePartition.end())
                        );
                    }
                });
            }
        }

        // 4. Sum up unique elements across all merged partitions
        [[maybe_unused]] auto t3 = std::chrono::high_resolution_clock::now();
        std::size_t totalUniqueWords = 0;
        for (std::size_t p = 0; p < NUM_PARTITIONS; ++p)
        {
            totalUniqueWords += threadResults[0].partitions[p].size();
        }
        [[maybe_unused]] auto t4 = std::chrono::high_resolution_clock::now();

        //std::println("chunking: {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count());
        //std::println("map:      {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
        //std::println("reduce:   {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count());
        //std::println("sum:      {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count());

        return totalUniqueWords;
    }
};


