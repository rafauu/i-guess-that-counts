#pragma once

#include <string_view>
#include <vector>

namespace Benchmark
{

struct Result
{
    std::string_view setName;
    std::size_t uniqueWords;
    double milliseconds;
};

std::vector<Result> runCartesianProduct(std::string_view fileMemory);
Result runOne(std::string_view fileMemory);

}
