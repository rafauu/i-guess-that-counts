module;

#if defined(__linux__) || defined(__unix__)
#include <sys/mman.h>
#include <unistd.h>
#endif

export module system_utils;

import std;

export namespace SystemUtils
{
    void adviseSequentialAccess(std::string_view memory) noexcept
    {
        if (memory.empty() or memory.data() == nullptr)
        {
            return;
        }

#if defined(__linux__) || defined(__unix__)
        auto callMadvise = [&] (auto flag) {
            ::madvise(
                const_cast<char*>(memory.data()),
                memory.size(),
                flag
            );
        };
        callMadvise(MADV_HUGEPAGE);
        callMadvise(MADV_SEQUENTIAL);
        callMadvise(MADV_WILLNEED);
#else
        (void)memory;
#endif
    }
}
