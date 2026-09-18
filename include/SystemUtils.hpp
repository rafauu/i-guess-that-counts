#pragma once

#include <string_view>

namespace SystemUtils
{
    void adviseSequentialAccess(std::string_view memory) noexcept;
}
