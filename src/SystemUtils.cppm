module;

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

export module system_utils;

import std;

export namespace SystemUtils
{

namespace Affinity
{

void pinThreadToHwCore(unsigned core_id) noexcept
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

}

void adviseHugePage(std::string_view memory) noexcept
{
    if (memory.empty() or memory.data() == nullptr)
    {
        return;
    }

    ::madvise(
        const_cast<char*>(memory.data()),
        memory.size(),
        MADV_HUGEPAGE
    );
}

}
