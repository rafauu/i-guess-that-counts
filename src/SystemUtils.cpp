#include "SystemUtils.hpp"

#if defined(__linux__) || defined(__unix__)
    #include <sys/mman.h>
#endif

namespace SystemUtils
{
    void adviseSequentialAccess(std::string_view memory) noexcept
    {
        if (memory.empty() or memory.data() == nullptr)
        {
            return;
        }

#if defined(__linux__) || defined(__unix__)
        ::madvise(
            const_cast<char*>(memory.data()),
            memory.size(),
            MADV_SEQUENTIAL | MADV_WILLNEED
        );
#else
        (void)memory;
#endif
    }
}
