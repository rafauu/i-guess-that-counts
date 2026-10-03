module;

#include <immintrin.h>

export module simd;

import std;

export namespace Simd
{
    [[nodiscard]] inline const char* skip_whitespace_avx2(
        const char* ptr,
        const char* end) noexcept
    {
        const __m256i whitespace = _mm256_set1_epi8(32);

        while (ptr + 32 <= end)
        {
            const __m256i data = _mm256_loadu_si256(
                reinterpret_cast<const __m256i*>(ptr)
            );

            // Unsigned max: max(byte, 32) == 32 for byte <= 32.
            const __m256i is_whitespace = _mm256_cmpeq_epi8(
                _mm256_max_epu8(data, whitespace),
                whitespace
            );

            const std::uint32_t mask = static_cast<std::uint32_t>(
                _mm256_movemask_epi8(is_whitespace)
            );

            if (mask != 0xFFFFFFFFu)
            {
                return ptr + std::countr_zero(~mask);
            }

            ptr += 32;
        }

        while (ptr < end && static_cast<unsigned char>(*ptr) <= 32)
        {
            ++ptr;
        }

        return ptr;
    }

    [[nodiscard]] inline const char* find_whitespace_avx2(
        const char* ptr,
        const char* end) noexcept
    {
        const __m256i whitespace = _mm256_set1_epi8(32);

        while (ptr + 32 <= end)
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

            if (mask != 0)
            {
                return ptr + std::countr_zero(mask);
            }

            ptr += 32;
        }

        while (ptr < end && static_cast<unsigned char>(*ptr) > 32)
        {
            ++ptr;
        }

        return ptr;
    }
}
