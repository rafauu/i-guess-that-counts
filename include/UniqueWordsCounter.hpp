#include <fmt/core.h>
#include <span>
#include <unordered_set>
#include <cctype>
#include <string>
#include <string_view>
#include "ConcurrentContainerFacade.hpp"
#include "ThreadPool.hpp"


constexpr auto operator""_MB(unsigned long long value)
{
    return value * 1024u * 1024u;
}

class UniqueWordsCounter
{
private:
    constexpr static unsigned long long MAX_CHUNK_SIZE = 1_MB;

    template <typename T>
    using UnderlyingStructure = std::unordered_set<T>;

    template <typename T>
    auto calculateChunkSize(std::span<const T> memory, unsigned long long offset) const
    {
        unsigned long long chunkSize;
        if (memory.size() - offset < MAX_CHUNK_SIZE)
        {
            chunkSize = memory.size() - offset;
        }
        else
        {
            chunkSize = MAX_CHUNK_SIZE;
            while (chunkSize > 0 and not std::isspace(memory[offset + chunkSize - 1]))
                --chunkSize;
        }
        return chunkSize;
    }

    auto getUniqueWords(std::string_view str) const
    {
        UnderlyingStructure<std::string> temp;

        static constexpr std::string_view delimiters{" \n"};
        size_t start;
        size_t end = 0;
        while ((start = str.find_first_not_of(delimiters, end)) != std::string_view::npos)
        {
            end = str.find_first_of(delimiters, start);
            temp.emplace(str.substr(start, end - start));
        }

        return temp;
    }

public:
    template <typename T>
    auto count(std::span<const T> memory) const
    {
        ConcurrentContainerFacade<UnderlyingStructure, std::string> words;
        ThreadPool threadPool;
        unsigned long long offset = 0;

        while(offset < memory.size())
        {
            if (threadPool.getTasksAmount() < threadPool.getThreadAmount())
            {
                auto chunkSize = calculateChunkSize(memory, offset);
                threadPool.pushTask([=, this, &words] {
                    words.insert(getUniqueWords({memory.data() + offset, chunkSize}));
                });

                fmt::print("Processing segment: {} - {}\n", offset, offset + chunkSize - 1);
                offset += chunkSize;
            }
        }

        threadPool.waitForTasks();
        return words.size();
    }
};
