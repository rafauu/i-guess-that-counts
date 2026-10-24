module;

#include <immintrin.h>

export module text_processor;

import std;

export namespace TextProcessor
{

[[nodiscard]] constexpr bool isWhitespace(char c) noexcept
{
    // simplified non-locale version of std::isspace/std::iscntrl
    return static_cast<unsigned char>(c) <= 32;
}

namespace Simd
{

struct WordRange
{
    const char* begin;
    const char* end;
};

[[nodiscard]] inline WordRange nextWord(const char* ptr, const char* end) noexcept
{
    const __m256i whitespace = _mm256_set1_epi8(32);

    // Phase 1: Scan for the beginning of a word
    while (end - ptr >= 32)
    {
        const __m256i data = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(ptr)
        );

        const __m256i is_whitespace = _mm256_cmpeq_epi8(
            _mm256_max_epu8(data, whitespace),
            whitespace
        );

        const std::uint32_t mask = static_cast<std::uint32_t>(
            _mm256_movemask_epi8(is_whitespace)
        );

        if (mask != 0xFFFFFFFFu)
        {
            // Found at least one non-whitespace character
            const std::uint32_t start_idx = static_cast<std::uint32_t>(std::countr_zero(~mask));
            const char* word_start = ptr + start_idx;

            // SINGLE-LOAD OPTIMIZATION:
            // Mask out the whitespace bits before `start_idx` to find the next whitespace.
            const std::uint32_t end_mask = mask & (0xFFFFFFFFu << start_idx);

            if (end_mask != 0)
            {
                // The word also ends in this same 32-byte chunk.
                // Calculate the end and return immediately (skipping phase 2 entirely).
                const std::uint32_t end_idx = static_cast<std::uint32_t>(std::countr_zero(end_mask));
                return {word_start, ptr + end_idx};
            }

            // The word continues past this 32-byte chunk
            ptr += 32;

            // Phase 2: Scan for the end of the word
            while (end - ptr >= 32)
            {
                const __m256i data2 = _mm256_loadu_si256(
                    reinterpret_cast<const __m256i*>(ptr)
                );

                const __m256i is_whitespace2 = _mm256_cmpeq_epi8(
                    _mm256_max_epu8(data2, whitespace),
                    whitespace
                );

                const std::uint32_t mask2 = static_cast<std::uint32_t>(
                    _mm256_movemask_epi8(is_whitespace2)
                );

                if (mask2 != 0)
                {
                    return {word_start, ptr + static_cast<std::uint32_t>(std::countr_zero(mask2))};
                }

                ptr += 32;
            }

            // Phase 2 scalar fallback (if end of string is reached while inside a word)
            while (ptr < end && static_cast<unsigned char>(*ptr) > 32)
            {
                ++ptr;
            }

            return {word_start, ptr};
        }

        ptr += 32;
    }

    // Phase 1 scalar fallback (for strings < 32 bytes or tail-ends with leading whitespace)
    while (ptr < end && static_cast<unsigned char>(*ptr) <= 32)
    {
        ++ptr;
    }

    if (ptr >= end)
    {
        return {end, end};
    }

    const char* word_start = ptr;

    // Phase 2 scalar fallback
    while (ptr < end && static_cast<unsigned char>(*ptr) > 32)
    {
        ++ptr;
    }

    return {word_start, ptr};
}

}
}
