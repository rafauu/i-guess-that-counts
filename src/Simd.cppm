module;

#include <immintrin.h>

export module simd;

import std;

export namespace Simd
{
    const char* skip_whitespace_avx2(const char* ptr, const char* end) noexcept {
        const __m256i v_space   = _mm256_set1_epi8(' ');
        const __m256i v_newline = _mm256_set1_epi8('\n');
        const __m256i v_tab     = _mm256_set1_epi8('\t');
        const __m256i v_cr      = _mm256_set1_epi8('\r');

        while (ptr + 32 <= end) {
            __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));

            __m256i is_space   = _mm256_cmpeq_epi8(data, v_space);
            __m256i is_newline = _mm256_cmpeq_epi8(data, v_newline);
            __m256i is_tab     = _mm256_cmpeq_epi8(data, v_tab);
            __m256i is_cr      = _mm256_cmpeq_epi8(data, v_cr);

            __m256i is_delim   = _mm256_or_si256(_mm256_or_si256(is_space, is_newline),
                                                _mm256_or_si256(is_tab, is_cr));

            std::uint32_t mask = _mm256_movemask_epi8(is_delim);

            // If not all 32 bytes are whitespace (~mask != 0), find the first non-whitespace
            if (mask != 0xFFFFFFFFu) {
                return ptr + __builtin_ctz(~mask);
            }
            ptr += 32;
        }

        while (ptr < end && (*ptr == ' ' || *ptr == '\n' || *ptr == '\t' || *ptr == '\r')) {
            ++ptr;
        }
        return ptr;
    }

    const char* find_whitespace_avx2(const char* ptr, const char* end) noexcept {
        const __m256i v_space   = _mm256_set1_epi8(' ');
        const __m256i v_newline = _mm256_set1_epi8('\n');
        const __m256i v_tab     = _mm256_set1_epi8('\t');
        const __m256i v_cr      = _mm256_set1_epi8('\r');

        while (ptr + 32 <= end) {
            __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));

            __m256i is_space   = _mm256_cmpeq_epi8(data, v_space);
            __m256i is_newline = _mm256_cmpeq_epi8(data, v_newline);
            __m256i is_tab     = _mm256_cmpeq_epi8(data, v_tab);
            __m256i is_cr      = _mm256_cmpeq_epi8(data, v_cr);

            __m256i is_delim   = _mm256_or_si256(_mm256_or_si256(is_space, is_newline),
                                                _mm256_or_si256(is_tab, is_cr));

            std::uint32_t mask = _mm256_movemask_epi8(is_delim);

            // If any byte matches a delimiter, find its index
            if (mask != 0) {
                return ptr + __builtin_ctz(mask);
            }
            ptr += 32;
        }

        while (ptr < end && *ptr != ' ' && *ptr != '\n' && *ptr != '\t' && *ptr != '\r') {
            ++ptr;
        }
        return ptr;
    }
}
