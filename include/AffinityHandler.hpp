#pragma once

struct AffinityHandler
{
    static void pinThreadToHwCore(unsigned core_id) noexcept;
};
