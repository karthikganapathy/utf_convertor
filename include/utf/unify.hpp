#ifndef _UNICODE_HPP__
#define _UNICODE_HPP__

#include <algorithm>
#include <execution>
#include <iostream>

#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>

#include "util/helper_functions.hpp"

namespace utf
{

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest  = true,
            bool BigEndianSrc   = true,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr        = nullptr
        >
class UniFy
{
public:
    using Result    = std::tuple<int64_t, int64_t>;
    [[nodiscard]] static Result transcode(DestType* output, const SrcType* input, const int64_t size) noexcept;

protected:
    static void magnify(DestType* output, const SrcType* input, const int64_t size) noexcept;
    [[nodiscard]] static int64_t verify(DestType* output, const int64_t size) noexcept;
    static void modify_u8(DestType* output, const int64_t size) noexcept;
    static void modify(DestType* output, const int64_t size) noexcept;
    [[nodiscard]] static Result qualify(DestType* output, const int64_t size) noexcept;

    static void native_u8_read_to32(DestType* output, const SrcType* input, const int64_t size) noexcept;
    static void native_u8_read_to16(DestType* output, const SrcType* input, const int64_t size) noexcept;
    static void native_u16_read(DestType* output, const SrcType* input, const int64_t size) noexcept;
    static void alien_u16_read(DestType* output, const SrcType* input, const int64_t size) noexcept;
    static void native_u32_read(DestType* output, const SrcType* input, const int64_t size) noexcept;
    static void alien_u32_read(DestType* output, const SrcType* input, const int64_t size) noexcept;

    static void get_halves16(__m256i& input_block, __m256i& first_half, __m256i& second_half, __m256i& zero) noexcept;
    static void get_halves32(__m256i& input_block, __m256i& first_half, __m256i& second_half, __m256i& zero) noexcept;
    static void identify_multibyte_seq(__m256i& input_block, __m256i& four_bytes_seq, __m256i& three_bytes_seq, __m256i& two_bytes_seq, __m256i& four_bytes_mask, __m256i& three_bytes_mask, __m256i& two_bytes_mask) noexcept;
    static void fix_two_bytes_seq(__m256i& data, __m256i& mask) noexcept;
    static void fix_three_bytes_seq(__m256i& data, __m256i& mask) noexcept;
    static void fix_four_bytes_seq(__m256i& data, __m256i& mask) noexcept;
    static void fix_bytes_seq_boundary(__m256i& data_low, __m256i& data_high, __m256i& mask) noexcept;

    static void set_two_bytes_seq(__m256i& first_qtr, __m256i& second_qtr, __m256i& third_qtr, __m256i& forth_qtr, __m256i& two_bytes_seq1, __m256i& two_bytes_seq2, __m256i& two_bytes_seq3, __m256i& two_bytes_seq4, __m256i& zero) noexcept;
    static void set_three_bytes_seq(__m256i& first_qtr, __m256i& second_qtr, __m256i& third_qtr, __m256i& forth_qtr, __m256i& three_bytes_seq1, __m256i& three_bytes_seq2, __m256i& three_bytes_seq3, __m256i& three_bytes_seq4, __m256i& zero) noexcept;
    static void set_four_bytes_seq(__m256i& first_qtr, __m256i& second_qtr, __m256i& third_qtr, __m256i& forth_qtr, __m256i& four_bytes_seq1, __m256i& four_bytes_seq2, __m256i& four_bytes_seq3, __m256i& four_bytes_seq4, __m256i& zero) noexcept;

    static bool get_u16_halves(__m256i& input_block, __m256i& first_half, __m256i& second_half, __m256i& zero, __m256i& high_bits_mask, __m256i& high_surrogate_mask) noexcept;

private:
    static constexpr uint8_t k_Convertion_Factor = sizeof(char32_t) / sizeof(DestType);
};

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
[[nodiscard]] UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::Result
UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::transcode(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    magnify(output, input, size);

    //return Result(size * 4, size / k_Convertion_Factor);
    int64_t invalid_index  = verify(output, size);

    modify(output, invalid_index);

    return qualify(output, invalid_index);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::get_halves16(__m256i& input_block, __m256i& first_half, __m256i& second_half, __m256i& zero) noexcept
{
    __m256i low_block, high_block;

    if constexpr (util::Endian::k_Little_Endian)
    {
        low_block       = _mm256_unpacklo_epi8(input_block, zero);
        high_block      = _mm256_unpackhi_epi8(input_block, zero);
    }
    else
    {
        low_block       = _mm256_unpacklo_epi8(zero, input_block);
        high_block      = _mm256_unpackhi_epi8(zero, input_block);
    }

    __m128i first_low   = _mm256_extracti128_si256(low_block, 0);
    __m128i first_high  = _mm256_extracti128_si256(high_block, 0);
    first_half          = _mm256_set_m128i(first_high, first_low);

    __m128i second_low  = _mm256_extracti128_si256(low_block, 1);
    __m128i second_high = _mm256_extracti128_si256(high_block, 1);
    second_half         = _mm256_set_m128i(second_high, second_low);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::get_halves32(__m256i& input_block, __m256i& first_half, __m256i& second_half, __m256i& zero) noexcept
{
    __m256i low_block, high_block;

    if constexpr (util::Endian::k_Little_Endian)
    {
        low_block       = _mm256_unpacklo_epi16(input_block, zero);
        high_block      = _mm256_unpackhi_epi16(input_block, zero);
    }
    else
    {
        low_block       = _mm256_unpacklo_epi16(zero, input_block);
        high_block      = _mm256_unpackhi_epi16(zero, input_block);
    }

    __m128i first_low   = _mm256_extracti128_si256(low_block, 0);
    __m128i first_high  = _mm256_extracti128_si256(high_block, 0);
    first_half          = _mm256_set_m128i(first_high, first_low);

    __m128i second_low  = _mm256_extracti128_si256(low_block, 1);
    __m128i second_high = _mm256_extracti128_si256(high_block, 1);
    second_half         = _mm256_set_m128i(second_high, second_low);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::identify_multibyte_seq(__m256i& input_block, __m256i& four_bytes_seq, __m256i& three_bytes_seq, __m256i& two_bytes_seq, __m256i& four_bytes_mask, __m256i& three_bytes_mask, __m256i& two_bytes_mask) noexcept
{
    __m256i high_bits       = _mm256_and_si256(input_block, four_bytes_mask);

    four_bytes_seq          = _mm256_cmpeq_epi8(high_bits, four_bytes_mask);

    high_bits               = _mm256_andnot_si256(four_bytes_seq, high_bits);
    high_bits               = _mm256_and_si256(high_bits, three_bytes_mask);

    three_bytes_seq         = _mm256_cmpeq_epi8(high_bits, three_bytes_mask);

    high_bits               = _mm256_andnot_si256(three_bytes_seq, high_bits);
    high_bits               = _mm256_and_si256(high_bits, two_bytes_mask);

    two_bytes_seq           = _mm256_cmpeq_epi8(high_bits, two_bytes_mask);

    //high_bits               = _mm256_andnot_si256(two_bytes_seq, high_bits);

    __m256i tmp_seq         = _mm256_bslli_epi128(four_bytes_seq, 1);
    auto tmp_boundary_value = _mm256_extract_epi8(four_bytes_seq, 15);
    tmp_seq                 = _mm256_insert_epi8(tmp_seq, tmp_boundary_value, 16);
    three_bytes_seq         = _mm256_or_si256(tmp_seq, three_bytes_seq);

    tmp_seq                 = _mm256_bslli_epi128(three_bytes_seq, 1);
    tmp_boundary_value      = _mm256_extract_epi8(three_bytes_seq, 15);
    tmp_seq                 = _mm256_insert_epi8(tmp_seq, tmp_boundary_value, 16);
    two_bytes_seq           = _mm256_or_si256(tmp_seq, two_bytes_seq);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::fix_two_bytes_seq(__m256i& data, __m256i& mask) noexcept
{
    __m256i sans_two_byte_seq   = _mm256_andnot_si256(mask, data);

    auto boundary_value         = _mm256_extract_epi32(sans_two_byte_seq, 4);
    __m256i tmp_bytes_shift     = _mm256_bsrli_epi128(sans_two_byte_seq, 4);
    tmp_bytes_shift             = _mm256_insert_epi32(tmp_bytes_shift, boundary_value, 3);

    __m256i two_bytes_seq_data  = _mm256_slli_epi32(data, 8);
    two_bytes_seq_data          = _mm256_or_si256(two_bytes_seq_data, tmp_bytes_shift);
    two_bytes_seq_data          = _mm256_and_si256(mask, two_bytes_seq_data);

    data                        = _mm256_or_si256(two_bytes_seq_data, sans_two_byte_seq);

    boundary_value              = _mm256_extract_epi32(mask, 3);
    tmp_bytes_shift             = _mm256_bslli_epi128(mask, 4);
    tmp_bytes_shift             = _mm256_insert_epi32(tmp_bytes_shift, boundary_value, 4);

    sans_two_byte_seq           = _mm256_andnot_si256(tmp_bytes_shift, data);
    data                        = _mm256_or_si256(sans_two_byte_seq, tmp_bytes_shift);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::fix_three_bytes_seq(__m256i& data, __m256i& mask) noexcept
{
    __m256i sans_three_byte_seq     = _mm256_andnot_si256(mask, data);

    auto boundary_value             = _mm256_extract_epi32(sans_three_byte_seq, 4);
    __m256i tmp_bytes_shift         = _mm256_bsrli_epi128(sans_three_byte_seq, 4);
    tmp_bytes_shift                 = _mm256_insert_epi32(tmp_bytes_shift, boundary_value, 3);

    __m256i three_bytes_seq_data    = _mm256_slli_epi32(data, 16);
    three_bytes_seq_data            = _mm256_or_si256(three_bytes_seq_data, tmp_bytes_shift);
    three_bytes_seq_data            = _mm256_and_si256(mask, three_bytes_seq_data);

    data                            = _mm256_or_si256(three_bytes_seq_data, sans_three_byte_seq);

    boundary_value                  = _mm256_extract_epi32(mask, 3);
    tmp_bytes_shift                 = _mm256_bslli_epi128(mask, 4);
    tmp_bytes_shift                 = _mm256_insert_epi32(tmp_bytes_shift, boundary_value, 4);

    sans_three_byte_seq             = _mm256_andnot_si256(tmp_bytes_shift, data);
    data                            = _mm256_or_si256(sans_three_byte_seq, tmp_bytes_shift);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::fix_four_bytes_seq(__m256i& data, __m256i& mask) noexcept
{
    __m256i sans_four_byte_seq      = _mm256_andnot_si256(mask, data);

    auto boundary_value             = _mm256_extract_epi32(sans_four_byte_seq, 4);
    __m256i tmp_bytes_shift         = _mm256_bsrli_epi128(sans_four_byte_seq, 4);
    tmp_bytes_shift                 = _mm256_insert_epi32(tmp_bytes_shift, boundary_value, 3);

    __m256i four_bytes_seq_data     = _mm256_slli_epi32(data, 24);
    four_bytes_seq_data             = _mm256_or_si256(four_bytes_seq_data, tmp_bytes_shift);
    four_bytes_seq_data             = _mm256_and_si256(mask, four_bytes_seq_data);

    data                            = _mm256_or_si256(four_bytes_seq_data, sans_four_byte_seq);

    boundary_value                  = _mm256_extract_epi32(mask, 3);
    tmp_bytes_shift                 = _mm256_bslli_epi128(mask, 4);
    tmp_bytes_shift                 = _mm256_insert_epi32(tmp_bytes_shift, boundary_value, 4);

    sans_four_byte_seq              = _mm256_andnot_si256(tmp_bytes_shift, data);
    data                            = _mm256_or_si256(sans_four_byte_seq, tmp_bytes_shift);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::fix_bytes_seq_boundary(__m256i& data_low, __m256i& data_high, __m256i& mask) noexcept
{
    auto last_seq_mask  = _mm256_extract_epi32(mask, 7);

    if (last_seq_mask == -1)
    {
        auto boundary_value     = _mm256_extract_epi32(data_high, 0);
        auto last_value         = _mm256_extract_epi32(data_low, 7) | boundary_value;
        data_low                = _mm256_insert_epi32(data_low, last_value, 7);
        data_high               = _mm256_insert_epi32(data_high, last_seq_mask, 0);
    }
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::set_two_bytes_seq(__m256i& first_qtr, __m256i& second_qtr, __m256i& third_qtr, __m256i& forth_qtr, __m256i& two_bytes_seq1, __m256i& two_bytes_seq2, __m256i& two_bytes_seq3, __m256i& two_bytes_seq4, __m256i& zero) noexcept
{
    if (_mm256_testc_si256(zero, two_bytes_seq1) != 1)
        fix_two_bytes_seq(first_qtr, two_bytes_seq1);
    if (_mm256_testc_si256(zero, two_bytes_seq2) != 1)
        fix_two_bytes_seq(second_qtr, two_bytes_seq2);
    if (_mm256_testc_si256(zero, two_bytes_seq3) != 1)
        fix_two_bytes_seq(third_qtr, two_bytes_seq3);
    if (_mm256_testc_si256(zero, two_bytes_seq4) != 1)
        fix_two_bytes_seq(forth_qtr, two_bytes_seq4);

    fix_bytes_seq_boundary(first_qtr, second_qtr, two_bytes_seq1);
    fix_bytes_seq_boundary(second_qtr, third_qtr, two_bytes_seq2);
    fix_bytes_seq_boundary(third_qtr, forth_qtr, two_bytes_seq3);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::set_three_bytes_seq(__m256i& first_qtr, __m256i& second_qtr, __m256i& third_qtr, __m256i& forth_qtr, __m256i& three_bytes_seq1, __m256i& three_bytes_seq2, __m256i& three_bytes_seq3, __m256i& three_bytes_seq4, __m256i& zero) noexcept
{
    if (_mm256_testc_si256(zero, three_bytes_seq1) != 1)
        fix_three_bytes_seq(first_qtr, three_bytes_seq1);
    if (_mm256_testc_si256(zero, three_bytes_seq2) != 1)
        fix_three_bytes_seq(second_qtr, three_bytes_seq2);
    if (_mm256_testc_si256(zero, three_bytes_seq3) != 1)
        fix_three_bytes_seq(third_qtr, three_bytes_seq3);
    if (_mm256_testc_si256(zero, three_bytes_seq4) != 1)
        fix_three_bytes_seq(forth_qtr, three_bytes_seq4);

    fix_bytes_seq_boundary(first_qtr, second_qtr, three_bytes_seq1);
    fix_bytes_seq_boundary(second_qtr, third_qtr, three_bytes_seq2);
    fix_bytes_seq_boundary(third_qtr, forth_qtr, three_bytes_seq3);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::set_four_bytes_seq(__m256i& first_qtr, __m256i& second_qtr, __m256i& third_qtr, __m256i& forth_qtr, __m256i& four_bytes_seq1, __m256i& four_bytes_seq2, __m256i& four_bytes_seq3, __m256i& four_bytes_seq4, __m256i& zero) noexcept
{
    if (_mm256_testc_si256(zero, four_bytes_seq1) != 1)
        fix_four_bytes_seq(first_qtr, four_bytes_seq1);
    if (_mm256_testc_si256(zero, four_bytes_seq2) != 1)
        fix_four_bytes_seq(second_qtr, four_bytes_seq2);
    if (_mm256_testc_si256(zero, four_bytes_seq3) != 1)
        fix_four_bytes_seq(third_qtr, four_bytes_seq3);
    if (_mm256_testc_si256(zero, four_bytes_seq4) != 1)
        fix_four_bytes_seq(forth_qtr, four_bytes_seq4);

    fix_bytes_seq_boundary(first_qtr, second_qtr, four_bytes_seq1);
    fix_bytes_seq_boundary(second_qtr, third_qtr, four_bytes_seq2);
    fix_bytes_seq_boundary(third_qtr, forth_qtr, four_bytes_seq3);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::native_u8_read_to32(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    __m256i zero                = _mm256_setzero_si256();

    __m256i four_bytes_mask     = _mm256_set1_epi8(static_cast<char>(0xF0));
    __m256i three_bytes_mask    = _mm256_set1_epi8(static_cast<char>(0xE0));
    __m256i two_bytes_mask      = _mm256_set1_epi8(static_cast<char>(0xC0));

    int64_t index   = 0;

    while (index + 32 < size)
    {
        __m256i input_block     = _mm256_loadu_si256((const __m256i*)(input + index));

        __m256i first_half, second_half, first_qtr, second_qtr, third_qtr, forth_qtr;

        get_halves16(input_block, first_half, second_half, zero);
        get_halves32(first_half, first_qtr, second_qtr, zero);
        get_halves32(second_half, third_qtr, forth_qtr, zero);

        __m256i four_bytes_seq, three_bytes_seq, two_bytes_seq;

        identify_multibyte_seq(input_block, four_bytes_seq, three_bytes_seq, two_bytes_seq, four_bytes_mask, three_bytes_mask, two_bytes_mask);

        int decrement = 0;

        if (    (_mm256_testc_si256(zero, two_bytes_seq) != 1)  ||
                (_mm256_testc_si256(zero, three_bytes_seq) != 1)  ||
                (_mm256_testc_si256(zero, four_bytes_seq) != 1)  )
        {
            __m256i first_half_4bs, second_half_4bs;
            __m256i first_half_3bs, second_half_3bs;
            __m256i first_half_2bs, second_half_2bs;

            __m256i four_bytes_seq1, four_bytes_seq2;
            __m256i three_bytes_seq1, three_bytes_seq2;
            __m256i two_bytes_seq1, two_bytes_seq2;

            __m256i four_bytes_seq3, four_bytes_seq4;
            __m256i three_bytes_seq3, three_bytes_seq4;
            __m256i two_bytes_seq3, two_bytes_seq4;

            if (_mm256_testc_si256(zero, four_bytes_seq) != 1)
            {
                get_halves16(four_bytes_seq, first_half_4bs, second_half_4bs, four_bytes_seq);
                get_halves16(three_bytes_seq, first_half_3bs, second_half_3bs, three_bytes_seq);
                get_halves16(two_bytes_seq, first_half_2bs, second_half_2bs, two_bytes_seq);

                get_halves32(first_half_4bs, four_bytes_seq1, four_bytes_seq2, first_half_4bs);
                get_halves32(first_half_3bs, three_bytes_seq1, three_bytes_seq2, first_half_3bs);
                get_halves32(first_half_2bs, two_bytes_seq1, two_bytes_seq2, first_half_2bs);

                get_halves32(second_half_4bs, four_bytes_seq3, four_bytes_seq4, second_half_4bs);
                get_halves32(second_half_3bs, three_bytes_seq3, three_bytes_seq4, second_half_3bs);
                get_halves32(second_half_2bs, two_bytes_seq3, two_bytes_seq4, second_half_2bs);

                set_two_bytes_seq(first_qtr, second_qtr, third_qtr, forth_qtr, two_bytes_seq1, two_bytes_seq2, two_bytes_seq3, two_bytes_seq4, zero);

                set_three_bytes_seq(first_qtr, second_qtr, third_qtr, forth_qtr, three_bytes_seq1, three_bytes_seq2, three_bytes_seq3, three_bytes_seq4, zero);

                set_four_bytes_seq(first_qtr, second_qtr, third_qtr, forth_qtr, four_bytes_seq1, four_bytes_seq2, four_bytes_seq3, four_bytes_seq4, zero);
            }
            else if (_mm256_testc_si256(zero, three_bytes_seq) != 1)
            {
                get_halves16(three_bytes_seq, first_half_3bs, second_half_3bs, three_bytes_seq);
                get_halves16(two_bytes_seq, first_half_2bs, second_half_2bs, two_bytes_seq);

                get_halves32(first_half_3bs, three_bytes_seq1, three_bytes_seq2, first_half_3bs);
                get_halves32(first_half_2bs, two_bytes_seq1, two_bytes_seq2, first_half_2bs);

                get_halves32(second_half_3bs, three_bytes_seq3, three_bytes_seq4, second_half_3bs);
                get_halves32(second_half_2bs, two_bytes_seq3, two_bytes_seq4, second_half_2bs);

                set_two_bytes_seq(first_qtr, second_qtr, third_qtr, forth_qtr, two_bytes_seq1, two_bytes_seq2, two_bytes_seq3, two_bytes_seq4, zero);

                set_three_bytes_seq(first_qtr, second_qtr, third_qtr, forth_qtr, three_bytes_seq1, three_bytes_seq2, three_bytes_seq3, three_bytes_seq4, zero);
            }
            else if (_mm256_testc_si256(zero, two_bytes_seq) != 1)
            {
                get_halves16(two_bytes_seq, first_half_2bs, second_half_2bs, two_bytes_seq);

                get_halves32(first_half_2bs, two_bytes_seq1, two_bytes_seq2, first_half_2bs);

                get_halves32(second_half_2bs, two_bytes_seq3, two_bytes_seq4, second_half_2bs);

                set_two_bytes_seq(first_qtr, second_qtr, third_qtr, forth_qtr, two_bytes_seq1, two_bytes_seq2, two_bytes_seq3, two_bytes_seq4, zero);
            }

            if (_mm256_extract_epi32(four_bytes_seq4, 5) == -1)
                decrement = 3;
            else if (   (_mm256_extract_epi32(four_bytes_seq4, 6) == -1)    ||
                        (_mm256_extract_epi32(three_bytes_seq4, 6) == -1)   )
                decrement = 2;
            else if (   (_mm256_extract_epi32(four_bytes_seq4, 7) == -1)    ||
                        (_mm256_extract_epi32(three_bytes_seq4, 7) == -1)   ||
                        (_mm256_extract_epi32(two_bytes_seq4, 7) == -1)     )
                decrement = 1;
        }

        _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index), first_qtr);
        _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index + 8), second_qtr);
        _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index + 16), third_qtr);
        _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index + 24), forth_qtr);

        index += 32;

        index -= decrement;
    }

    while (index < size)
    {
        //*(reinterpret_cast<char32_t*>(output) + index)  = static_cast<char32_t>(*(input + index));
        char32_t value      = static_cast<char32_t>(*(input + index));

        uint8_t num         = static_cast<uint8_t>(value & 0x000000FF);
        uint8_t num_bytes   = 0;

        while (num & 0x80)
        {
            num_bytes++;
            num = num << 1;
        }

        // last incomplete element
        if (index + num_bytes > size)
        {
            *(reinterpret_cast<char32_t*>(output) + index++) = value;

            while (index < size)
                *(reinterpret_cast<char32_t*>(output) + index++) = 0xFFFFFFFF;

            return;
        }

        uint8_t ctr = 0;
        char32_t cu = value;

        while (++ctr < num_bytes)
        {
            char32_t next_cu = static_cast<char32_t>(*(input + index + ctr));
            cu = ( (cu << 8) | (next_cu & 0x000000FF) ); 
            *(reinterpret_cast<char32_t*>(output) + index + ctr) = 0xFFFFFFFF;
        }

        *(reinterpret_cast<char32_t*>(output) + index)  = cu;

        index += ctr;
    }
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
bool UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::get_u16_halves(__m256i& input_block, __m256i& first_half, __m256i& second_half, __m256i& zero, __m256i& high_bits_mask, __m256i& high_surrogate_mask) noexcept
{
    get_halves32(input_block, first_half, second_half, zero);

    __m256i high_bits   = _mm256_and_si256(input_block, high_bits_mask);
    __m256i surrogates  = _mm256_cmpeq_epi16(high_bits, high_surrogate_mask);

    auto mask0          = _mm256_extract_epi64(surrogates, 0);
    auto mask1          = _mm256_extract_epi64(surrogates, 1);
    auto mask2          = _mm256_extract_epi64(surrogates, 2);
    auto mask3a         = _mm256_extract_epi32(surrogates, 7);
    auto mask3b1        = _mm256_extract_epi16(surrogates, 14);
    auto mask3b2        = _mm256_extract_epi16(surrogates, 15);

    __m256i surrogates1 = zero;
    __m256i surrogates2 = zero;

    __m256i surrogates_set1     = _mm256_unpacklo_epi16(surrogates, surrogates);
    __m256i surrogates_set2     = _mm256_unpackhi_epi16(surrogates, surrogates);

    if (mask0 != 0 || mask1 != 0)
    {
        __m128i first_set1          = _mm256_extracti128_si256(surrogates_set1, 0);
        __m128i first_set2          = _mm256_extracti128_si256(surrogates_set2, 0);
        surrogates1                 = _mm256_set_m128i(first_set2, first_set1);

        __m256i first_half_shift    = _mm256_slli_epi32(first_half, 16);
        first_half_shift            = _mm256_and_si256(surrogates1, first_half_shift);
        __m256i first_half_value    = _mm256_andnot_si256(surrogates1, first_half);
        first_half_shift            = _mm256_or_si256(first_half_value, first_half_shift);

        auto boundary_value         = _mm256_extract_epi32(first_half, 4);
        __m256i first_half_loshift  = _mm256_bsrli_epi128(first_half, 4);
        first_half_loshift          = _mm256_insert_epi32(first_half_loshift, boundary_value, 3);

        __m256i first_half_tmp      = _mm256_or_si256(first_half_shift, first_half_loshift);
        __m256i first_half_sgate    = _mm256_and_si256(surrogates1, first_half_tmp);

        first_half_tmp              = _mm256_andnot_si256(surrogates1, first_half);

        first_half_value            = _mm256_or_si256(first_half_tmp, first_half_sgate);

        if (_mm256_extract_epi32(surrogates1, 7) == -1)
        {
            boundary_value          = _mm256_extract_epi32(second_half, 0);
            auto last_value         = _mm256_extract_epi32(first_half_value, 7) | boundary_value;
            first_half_value        = _mm256_insert_epi32(first_half_value, last_value, 7);
        }

        boundary_value              = _mm256_extract_epi32(surrogates1, 3);
        __m256i surrogates1_prime   = _mm256_bslli_epi128(surrogates1, 4);
        surrogates1_prime           = _mm256_insert_epi32(surrogates1_prime, boundary_value, 4);

        first_half_tmp              = _mm256_andnot_si256(surrogates1_prime, first_half_value);
        first_half                  = _mm256_or_si256(first_half_tmp, surrogates1_prime);
    }

    if (mask2 != 0 || mask3a != 0 || mask3b1 != 0)
    {
        __m128i second_set1         = _mm256_extracti128_si256(surrogates_set1, 1);
        __m128i second_set2         = _mm256_extracti128_si256(surrogates_set2, 1);
        surrogates2                 = _mm256_set_m128i(second_set2, second_set1);

        __m256i second_half_shift   = _mm256_slli_epi32(second_half, 16);
        second_half_shift           = _mm256_and_si256(surrogates2, second_half_shift);
        __m256i second_half_value   = _mm256_andnot_si256(surrogates2, second_half);
        second_half_shift           = _mm256_or_si256(second_half_value, second_half_shift);

        auto boundary_value         = _mm256_extract_epi32(second_half, 4);
        __m256i second_half_loshift = _mm256_bsrli_epi128(second_half, 4);
        second_half_loshift         = _mm256_insert_epi32(second_half_loshift, boundary_value, 3);

        __m256i second_half_tmp     = _mm256_or_si256(second_half_shift, second_half_loshift);
        __m256i second_half_sgate   = _mm256_and_si256(surrogates2, second_half_tmp);

        second_half_tmp             = _mm256_andnot_si256(surrogates2, second_half);

        second_half_value           = _mm256_or_si256(second_half_tmp, second_half_sgate);

        auto surrogates1_last_value = _mm256_extract_epi32(surrogates1, 7);
        boundary_value              = _mm256_extract_epi32(surrogates2, 3);
        __m256i surrogates2_prime   = _mm256_bslli_epi128(surrogates2, 4);
        surrogates2_prime           = _mm256_insert_epi32(surrogates2_prime, boundary_value, 4);
        surrogates2_prime           = _mm256_insert_epi32(surrogates2_prime, surrogates1_last_value, 0);

        second_half_tmp             = _mm256_andnot_si256(surrogates2_prime, second_half_value);

        second_half                 = _mm256_or_si256(second_half_tmp, surrogates2_prime);
    }

    return (mask3b2 != 0);
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::native_u16_read(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    __m256i zero                = _mm256_setzero_si256();
    __m256i high_bits_mask      = _mm256_set1_epi16(static_cast<short int>(0xFC00));
    __m256i high_surrogate_mask = _mm256_set1_epi16(static_cast<short int>(0xD800));

    int64_t index   = 0;

    while (index + 16 < size)
    {
        __m256i input_block = _mm256_loadu_si256((const __m256i*)(input + index));

        if constexpr (std::is_same<char16_t, DestType>::value)
        {
            (void) zero;
            (void) high_bits_mask;
            (void) high_surrogate_mask;

            _mm256_storeu_si256((__m256i*)(output + index), input_block);

            index += 16;
        }
        else
        {
            __m256i first_half, second_half;

            bool skip_last  = get_u16_halves(input_block, first_half, second_half, zero, high_bits_mask, high_surrogate_mask);

            _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index), first_half);
            _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index + 8), second_half);

            index += 16;

            if (skip_last)
                index -= 1;
        }
    }

    while (index < size)
    {
        if constexpr (std::is_same<char16_t, DestType>::value)
        {
            output[index]   = *(input + index);
        }
        else
        {
            char32_t value  = static_cast<char32_t>(*(input + index));
            *(reinterpret_cast<char32_t*>(output) + index)  = value;

            if (((value & 0x0000FC00) == 0x0000D800) && (index + 1 != size))
            {
                char32_t next_value = static_cast<char32_t>(*(input + index + 1));
                *(reinterpret_cast<char32_t*>(output) + index)  = ( (value << 16) | (next_value & 0x0000FFFF) );
                index++;
                *(reinterpret_cast<char32_t*>(output) + index)  = 0xFFFFFFFF;
            }
        }

        index++;
    }
    /*
            uint8_t high_bits_print[32] = {0, };
            std::memcpy(&high_bits_print, &input_block, 32);
            std::cout << std::hex << "--[";
            for (int i = 0; i < 32; ++i)
                std::cout << (int)high_bits_print[i] << ", ";
            std::cout << "]\n";
    */
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::alien_u16_read(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    __m256i zero                = _mm256_setzero_si256();
    __m256i high_bits_mask      = _mm256_set1_epi16(static_cast<short int>(0xFC00));
    __m256i high_surrogate_mask = _mm256_set1_epi16(static_cast<short int>(0xD800));

    int64_t index   = 0;

    while (index + 16 < size)
    {
        __m256i input_block = _mm256_loadu_si256((const __m256i*)(input + index));

        __m256i left_bytes  = _mm256_slli_epi16(input_block, 8);
        __m256i right_bytes = _mm256_srli_epi16(input_block, 8);
        input_block         = _mm256_or_si256(left_bytes, right_bytes);

        if constexpr (std::is_same<char16_t, DestType>::value)
        {
            (void) zero;
            (void) high_bits_mask;
            (void) high_surrogate_mask;

            _mm256_storeu_si256((__m256i*)(output + index), input_block);

            index += 16;
        }
        else
        {
            __m256i first_half, second_half;

            bool skip_last  = get_u16_halves(input_block, first_half, second_half, zero, high_bits_mask, high_surrogate_mask);

            _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index), first_half);
            _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index + 8), second_half);

            index += 16;

            if (skip_last)
                index -= 1;
        }
    }

    while (index < size)
    {
        if constexpr (std::is_same<char16_t, DestType>::value)
        {
            char16_t value  = *(input + index);
            output[index]   = ( ((0x00FF & value) << 8) | ((0xFF00 & value) >> 8) );
        }
        else
        {
            char32_t value  = static_cast<char32_t>(*(input + index));
            value           = ( ((value & 0x0000FF00) >> 8 ) |
                                ((value & 0x000000FF) << 8 ) );

            *(reinterpret_cast<char32_t*>(output) + index)  = value;

            if (((value & 0x0000FC00) == 0x0000D800) && (index + 1 != size))
            {
                char32_t next_value = static_cast<char32_t>(*(input + index + 1));

                next_value          = ( ((next_value & 0x0000FF00) >> 8 ) |
                                        ((next_value & 0x000000FF) << 8 ) );

                *(reinterpret_cast<char32_t*>(output) + index)  = ( (value << 16) | (next_value & 0x0000FFFF) );
                index++;
                *(reinterpret_cast<char32_t*>(output) + index)  = 0xFFFFFFFF;
            }
        }

        index++;
    }
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::native_u32_read(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    int64_t index   = 0;
    int64_t r_size  = size & 0xFFFFFFFFFFFFFFF8;

    while (index < r_size)
    {
        __m256i input_block = _mm256_loadu_si256((const __m256i*)(input + index));
        _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index), input_block);
        index      += 8;
    }

    while (index < size)
    {
        *(reinterpret_cast<char32_t*>(output) + index)  = *(input + index);
        index++;
    }
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::alien_u32_read(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    int64_t index   = 0;
    int64_t r_size  = size & 0xFFFFFFFFFFFFFFF8;

    while (index < r_size)
    {
        __m256i input_block = _mm256_loadu_si256((const __m256i*)(input + index));

        __m256i left_bytes  = _mm256_slli_epi16(input_block, 8);
        __m256i right_bytes = _mm256_srli_epi16(input_block, 8);
        input_block         = _mm256_or_si256(left_bytes, right_bytes);

        left_bytes          = _mm256_slli_epi32(input_block, 16);
        right_bytes         = _mm256_srli_epi32(input_block, 16);
        input_block         = _mm256_or_si256(left_bytes, right_bytes);

        _mm256_storeu_si256((__m256i*)(reinterpret_cast<char32_t*>(output) + index), input_block);

        index              += 8;
    }

    while (index < size)
    {
        char32_t value = *(input + index);

        *(reinterpret_cast<char32_t*>(output) + index)  = ( ((value & 0xFF000000) >> 24) |
                                                            ((value & 0x00FF0000) >>  8) |
                                                            ((value & 0x0000FF00) <<  8) |
                                                            ((value & 0x000000FF) << 24) );

        index++;
    }
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::magnify(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    if constexpr (std::is_same<char8_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value)
    {
        int64_t index   = 0;
        int64_t r_size  = size & 0xFFFFFFFFFFFFFFE0;

        while (index < r_size)
        {
            __m256i input_block = _mm256_loadu_si256((const __m256i*)(input + index));
            _mm256_storeu_si256((__m256i*)(output + index), input_block);
            index  += 32;
        }

        while (index < size)
        {
            output[index] = input[index];
            index++;
        }
    }

    if constexpr (std::is_same<char16_t, DestType>::value && std::is_same<char8_t,  SrcType>::value)
    {
        return native_u8_read_to16(output, input, size);
    }

    if constexpr (std::is_same<char32_t, DestType>::value && std::is_same<char8_t,  SrcType>::value)
    {
        return native_u8_read_to32(output, input, size);
    }

    if constexpr (std::is_same<char16_t, SrcType>::value)
    {
        if constexpr (util::Endian::k_Little_Endian)
        {
            if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
            {
                return alien_u16_read(output, input, size);
            }
            else
            {
                return native_u16_read(output, input, size);
            }
        }
        else
        {
            if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
            {
                return native_u16_read(output, input, size);
            }
            else
            {
                return alien_u16_read(output, input, size);
            }
        }
    }

    if constexpr (std::is_same<char32_t, SrcType>::value)
    {
        if constexpr (util::Endian::k_Little_Endian)
        {
            if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
            {
                return alien_u32_read(output, input, size);
            }
            else
            {
                return native_u32_read(output, input, size);
            }
        }
        else
        {
            if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
            {
                return native_u32_read(output, input, size);
            }
            else
            {
                return alien_u32_read(output, input, size);
            }
        }
    }

    return;
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
[[nodiscard]] int64_t UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::verify(DestType* output, const int64_t size) noexcept
{
    if constexpr (
            (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        int64_t length  = size;

        if (length > 0 && (output[length - 1] & 0x80) == 0x0)
            return size;

        while ( length > 0 && ((output[length - 1] & 0x80) != 0x0) && ((output[length - 1] & 0xC0) != 0xC0) )
            length--;

        char8_t cu      = output[length];
        uint8_t num_cus = 0;

        while ((cu & 0x80) != 0x0)
        {
            num_cus++;
            cu = (cu << 1);
        }

        return (size - length) == (num_cus - 1) ? size : (length - 1);
    }
    else if constexpr (
            (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        char32_t*   output32_p  = reinterpret_cast<char32_t*>(output);
        int64_t     length      = size;

        if (length > 0 && (output32_p[length - 1] & 0xFFFFFF80) == 0x0)
            return size * k_Convertion_Factor;

        while (length > 0 && output32_p[length - 1] == 0xFFFFFFFF)
            length--;

        char32_t cu     = output32_p[length - 1];

        while ((cu & 0x80000000) == 0x0)
            cu = (cu << 1);

        uint8_t num_cus = 0;

        while ((cu & 0x80000000) != 0x0)
        {
            num_cus++;
            cu = (cu << 1);
        }

        return (size - length) == (num_cus - 1) ? size * k_Convertion_Factor : (length - 1) * k_Convertion_Factor;
    }
    else if constexpr (
            (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        int64_t length = size;

        if (length > 0 && (output[length - 1] & 0xFC00) == 0xD800)
            return length - 1;

        return length;
    }
    else if constexpr (
            (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
            (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);
        int64_t length          = size;

        if (length > 0 && (output32_p[length - 1] & 0xFC00) == 0xD800)
            return (length - 1) * k_Convertion_Factor;

        return length * k_Convertion_Factor;
    }
    else if constexpr (
            (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
            (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
            (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) )
    {
        (void) output;
        return size * k_Convertion_Factor;
    }
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::modify_u8(DestType* output, const int64_t size) noexcept
{
    __m256i zero                = _mm256_setzero_si256();
    __m256i empty_slots         = _mm256_set1_epi32(static_cast<int32_t>(0xFFFFFFFF));
    __m256i mbs_mask            = _mm256_set1_epi32(static_cast<int32_t>(0xFFFFFF80));
    __m256i last_6bits_mask     = _mm256_set1_epi32(static_cast<int32_t>(0x3F));
    __m256i last_12bits_mask    = _mm256_set1_epi32(static_cast<int32_t>(0xFFF));
    __m256i last_16bits_mask    = _mm256_set1_epi32(static_cast<int32_t>(0xFFFF));
    __m256i last_18bits_mask    = _mm256_set1_epi32(static_cast<int32_t>(0x3FFFF));
    __m256i last_22bits_mask    = _mm256_set1_epi32(static_cast<int32_t>(0x3FFFFF));
    __m256i mbs_3bytes_check    = _mm256_set1_epi32(static_cast<int32_t>(0xE0000));

    int64_t index               = 0;

    char32_t* output32_p        = reinterpret_cast<char32_t*>(output);

    while (index + 8 < size)
    {
        __m256i input_block         = _mm256_loadu_si256((const __m256i*)(output32_p + index));
        __m256i empty_mask          = _mm256_cmpeq_epi32(empty_slots, input_block);
        __m256i data                = _mm256_andnot_si256(empty_mask, input_block);
        __m256i empty_data          = _mm256_and_si256(empty_mask, input_block);

        __m256i mbs_data_tmp        = _mm256_and_si256(mbs_mask, data);
        __m256i ascii_data_mask     = _mm256_cmpeq_epi32(zero, mbs_data_tmp);

        __m256i ascii_data          = _mm256_and_si256(ascii_data_mask, data);
        __m256i mbs_data            = _mm256_andnot_si256(ascii_data_mask, data);

        if (_mm256_testc_si256(zero, mbs_data) == 1)
        {
            index += 8;
            continue;
        }

        __m256i last_bits           = _mm256_and_si256(last_6bits_mask, mbs_data);
        mbs_data                    = _mm256_srli_epi32(mbs_data, 2);
        mbs_data                    = _mm256_andnot_si256(last_6bits_mask, mbs_data);
        mbs_data                    = _mm256_or_si256(last_bits, mbs_data);

        last_bits                   = _mm256_and_si256(last_12bits_mask, mbs_data);
        mbs_data                    = _mm256_srli_epi32(mbs_data, 2);
        mbs_data                    = _mm256_andnot_si256(last_12bits_mask, mbs_data);
        mbs_data                    = _mm256_or_si256(last_bits, mbs_data);

        last_bits                   = _mm256_and_si256(mbs_3bytes_check, mbs_data);
        __m256i mbs_3bytes_mask     = _mm256_cmpeq_epi32(mbs_3bytes_check, last_bits);

        if (_mm256_testc_si256(zero, mbs_3bytes_mask) != 1)
        {
            __m256i mbs_3bytes_data     = _mm256_and_si256(mbs_3bytes_mask, mbs_data);
            mbs_data                    = _mm256_andnot_si256(mbs_3bytes_mask, mbs_data);

            mbs_3bytes_data             = _mm256_and_si256(last_16bits_mask, mbs_3bytes_data);
            mbs_data                    = _mm256_or_si256(mbs_3bytes_data, mbs_data);
        }

        __m256i mbs_4bytes_data     = _mm256_and_si256(last_16bits_mask, mbs_data);

        if (_mm256_testc_si256(zero, mbs_4bytes_data) != 1)
        {
            last_bits                   = _mm256_and_si256(last_18bits_mask, mbs_data);
            mbs_data                    = _mm256_srli_epi32(mbs_data, 2);
            mbs_data                    = _mm256_andnot_si256(last_18bits_mask, mbs_data);
            mbs_data                    = _mm256_or_si256(last_bits, mbs_data);

            mbs_data                    = _mm256_and_si256(last_22bits_mask, mbs_data);
            mbs_data                    = _mm256_or_si256(ascii_data, mbs_data);
            mbs_data                    = _mm256_or_si256(empty_data, mbs_data);
        }

        _mm256_storeu_si256((__m256i*)(output32_p + index), mbs_data);

        index += 8;
    }

    while (index < size)
    {
        char32_t value  = static_cast<char32_t>(*(output32_p + index));

        if (value != 0xFFFFFFFF && value > 0x0000007F)
        {
            char32_t code_point = 0x0;
            char32_t this_value = value;
            uint8_t  ctr        = 0;
            uint8_t  mask       = 0x3F;

            while ((this_value & 0x000000C0) == 0x00000080)
            {
                code_point     |= ( (this_value & 0x0000003F) << (6 * ctr++) );
                this_value      = (this_value >> 8);
                mask            = (mask >> 1);
            }

            code_point         |= ( (this_value & mask) << (6 * ctr) );

            if constexpr (std::is_same<char16_t, DestType>::value)
            {
                if (code_point > 0x00010000)
                {
                    code_point         -= 0x00010000;

                    char32_t new_cp     = 0;

                    new_cp              = ( (code_point & 0x000003FF) | 0x0000DC00 );
                    new_cp             |= ( ( ((code_point >> 10) & 0x000003FF) | 0x0000D800 ) << 16 );

                    code_point          = new_cp;
                }
            }

            *(reinterpret_cast<char32_t*>(output32_p) + index)  = code_point;
        }

        index++;
    }
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::modify(DestType* output, const int64_t size) noexcept
{
    if constexpr (
            (std::is_same<char16_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        modify_u8(output, size);
        /*
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto decode_u8  = [&output32_p](const char32_t& value)
            {
                if (value == 0xFFFFFFFF || value < 0x00000080)
                    return;

                char32_t code_point = 0x0;
                char32_t this_value = value;
                int64_t  index      = &value - output32_p;
                uint8_t  ctr        = 0;
                uint8_t  mask       = 0x3F;

                while ((this_value & 0x000000C0) == 0x00000080)
                {
                    code_point     |= ( (this_value & 0x0000003F) << (6 * ctr++) );
                    this_value      = (this_value >> 8);
                    mask            = (mask >> 1);
                }

                code_point         |= ( (this_value & mask) << (6 * ctr) );
                //code_point          = (code_point & 0x001FFFFF);

                if constexpr (std::is_same<char16_t, DestType>::value)
                {
                    if (code_point < 0x00010000)
                    {
                        output32_p[index]   = code_point;
                        return;
                    }

                    code_point         -= 0x00010000;

                    char32_t new_cp     = 0;

                    new_cp              = ( (code_point & 0x000003FF) | 0x0000DC00 );
                    new_cp             |= ( ( ((code_point >> 10) & 0x000003FF) | 0x0000D800 ) << 16 );

                    output32_p[index]   = new_cp;
                    return;
                }
                else
                {
                    output32_p[index]   = code_point;
                    return;
                }
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size / k_Convertion_Factor, decode_u8);
        */
    }
    else if constexpr (
            (std::is_same<char8_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto decode_u16 = [&output32_p](const char32_t& value)
            {
                if (value == 0xFFFFFFFF)
                    return;

                char32_t code_point = 0x0;
                int64_t  index      = &value - output32_p;

                if (value > 0x0000FFFF)
                {
                    code_point      = ( (value & 0x000003FF)      );
                    code_point     |= ( (value & 0x03FF0000) >> 6 ); // (16 - 10)
                    code_point     += 0x00010000;
                    //code_point      = (code_point & 0x001FFFFF);
                }
                else
                {
                    code_point      = value;
                }

                if constexpr (std::is_same<char8_t, DestType>::value)
                {
                    if (code_point < 0x80)
                    {
                        output32_p[index]   = code_point;
                        return;
                    }

                    char32_t    new_cu  = 0x0;
                    char8_t     cus[4]  = {0x0, };
                    uint8_t     cu_ctr  = 0;
                    uint16_t    mask    = 0xFF80;

                    while ((code_point & 0xFFFFFFC0) > 0x3F)
                    {
                        cus[cu_ctr++]   = ((code_point & 0x3F) | 0x80);
                        code_point      = (code_point >> 6);
                        mask            = (mask >> 1);
                    }

                    if ((cu_ctr == 1 && code_point > 0x1F) ||
                        (cu_ctr == 2 && code_point > 0x0F) )
                    {
                        cus[cu_ctr++]   = ((code_point & 0x3F) | 0x80);
                        code_point      = (code_point >> 6);
                        mask            = (mask >> 1);
                    }

                    cus[cu_ctr]         = (code_point | (mask & 0xFF));

                    for (int i = 0; i <= cu_ctr; ++i)
                        new_cu          = ((new_cu << 8) | cus[i]);

                    output32_p[index]   = new_cu;
                }
                else
                {
                    output32_p[index]   = code_point;
                }

                return;
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size / k_Convertion_Factor, decode_u16);
    }
    else if constexpr (
            (std::is_same<char8_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
            (std::is_same<char16_t, DestType>::value && std::is_same<char32_t, SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto decode_u32 = [&output32_p](const char32_t& value)
            {
                if constexpr (std::is_same<char16_t, DestType>::value)
                {
                    char32_t code_point = value;
                    int64_t  index      = &value - output32_p;

                    if (code_point < 0x00010000)
                    {
                        output32_p[index]   = code_point;
                        return;
                    }

                    code_point         -= 0x00010000;

                    char32_t new_cp     = 0;

                    new_cp              = ( (code_point & 0x000003FF) | 0x0000DC00 );
                    new_cp             |= ( ( ((code_point >> 10) & 0x000003FF) | 0x0000D800 ) << 16 );

                    output32_p[index]   = new_cp;
                }
                else
                {
                    char32_t code_point = value;
                    int64_t  index      = &value - output32_p;

                    if (code_point < 0x80)
                    {
                        output32_p[index]   = code_point;
                        return;
                    }

                    char32_t    new_cu  = 0x0;
                    char8_t     cus[4]  = {0x0, };
                    uint8_t     cu_ctr  = 0;
                    uint16_t    mask    = 0xFF80;

                    while ((code_point & 0xFFFFFFC0) > 0x3F)
                    {
                        cus[cu_ctr++]   = ((code_point & 0x3F) | 0x80);
                        code_point      = (code_point >> 6);
                        mask            = (mask >> 1);
                    }

                    if ((cu_ctr == 1 && code_point > 0x1F) ||
                        (cu_ctr == 2 && code_point > 0x0F) )
                    {
                        cus[cu_ctr++]   = ((code_point & 0x3F) | 0x80);
                        code_point      = (code_point >> 6);
                        mask            = (mask >> 1);
                    }

                    cus[cu_ctr]         = (code_point | (mask & 0xFF));

                    for (int i = 0; i <= cu_ctr; ++i)
                        new_cu          = ((new_cu << 8) | cus[i]);

                    output32_p[index]   = new_cu;
                }
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size / k_Convertion_Factor, decode_u32);
    }
    else
    {
        (void) output;
        (void) size;
    }

    return;
}

template<   typename DestType, 
            typename SrcType,
            bool BigEndianDest,
            bool BigEndianSrc,
            std::enable_if_t<
                (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
                (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) ||
                (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value),
                DestType*
            > t_dest_ptr
        >
[[nodiscard]] UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::Result
UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::qualify(DestType* output, const int64_t size) noexcept
{
    if constexpr (
            (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        // do nothing
        (void) output;
        return Result(size, size);
    }
    else if constexpr (
            (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        int64_t index       = 0;

        while (index < size)
        {
            char16_t cu     = output[index];

            if constexpr (
                    ( util::Endian::k_Little_Endian &&
                      std::is_same<std::true_type,  std::integral_constant<bool, BigEndianDest>>::value ) ||
                    ( !util::Endian::k_Little_Endian &&
                      std::is_same<std::false_type, std::integral_constant<bool, BigEndianDest>>::value )
                        )
            {
                cu          = ((cu >> 8) | ((cu & 0x00FF) << 8));
            }

            output[index]   = cu;

            index++;
        }

        return Result(size, size);
    }
    else if constexpr (
            (std::is_same<char32_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) )
    {
        int64_t index       = 0;

        while (index < size)
        {
            char32_t cu     = output[index];

            if constexpr (
                    ( util::Endian::k_Little_Endian &&
                      std::is_same<std::true_type,  std::integral_constant<bool, BigEndianDest>>::value ) ||
                    ( !util::Endian::k_Little_Endian &&
                      std::is_same<std::false_type, std::integral_constant<bool, BigEndianDest>>::value )
                        )
            {
                cu          = ( ( (cu & 0xFF000000) >> 24 ) |
                                ( (cu & 0x00FF0000) >>  8 ) |
                                ( (cu & 0x0000FF00) <<  8 ) |
                                ( (cu & 0x000000FF) << 24 ) );
            }

            output[index]   = cu;

            index++;
        }

        return Result(size, size / k_Convertion_Factor);
    }
    else if constexpr (
            (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
            (std::is_same<char8_t,   DestType>::value && std::is_same<char32_t, SrcType>::value) )
    {
        int64_t index       = 0;
        int64_t ctr         = 0;
        int64_t dest_size   = size / k_Convertion_Factor;

        while (index < dest_size)
        {
            char32_t code_point = *(reinterpret_cast<char32_t*>(output) + index++);

            if (code_point == 0xFFFFFFFF)
                continue;

            //if (code_point < 0x00000080)
            //{
            //    output[ctr++]   = static_cast<DestType>(code_point);
            //    continue;
            //}

            while (code_point > 0x0)
            {
                output[ctr++]   = static_cast<DestType>(code_point & 0xFF);
                code_point      = (code_point >> 8);
            }
        }

        return Result(ctr, dest_size);
    }
    else if constexpr (
            (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char16_t,  DestType>::value && std::is_same<char32_t, SrcType>::value) )
    {
        int64_t index       = 0;
        int64_t ctr         = 0;
        int64_t dest_size   = size / k_Convertion_Factor;

        while (index < dest_size)
        {
            char32_t code_point = *(reinterpret_cast<char32_t*>(output) + index++);

            if (code_point == 0xFFFFFFFF)
                continue;

            if (code_point == 0x0)
            {
                output[ctr++]       = 0x0;
                continue;
            }

            for (int i = 1; i >= 0; --i)
            {
                char16_t cu         = ( (code_point & (0xFFFF << (i * 16))) >> (i * 16) );

                if constexpr (
                        ( util::Endian::k_Little_Endian &&
                          std::is_same<std::true_type,  std::integral_constant<bool, BigEndianDest>>::value ) ||
                        ( !util::Endian::k_Little_Endian &&
                          std::is_same<std::false_type, std::integral_constant<bool, BigEndianDest>>::value )
                            )
                {
                    cu              = ((cu >> 8) | ((cu & 0x00FF) << 8));
                }

                if (cu != 0x0)
                    output[ctr++]   = cu;
            }
        }

        return Result(ctr, dest_size);
    }
    else if constexpr (
            (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        int64_t index       = 0;
        int64_t ctr         = 0;

        while (index < size)
        {
            char32_t code_unit = output[index++];

            if (code_unit == 0xFFFFFFFF)
                continue;

            if constexpr (
                    ( util::Endian::k_Little_Endian &&
                      std::is_same<std::true_type, std::integral_constant<bool, BigEndianDest>>::value  ) ||
                    ( !util::Endian::k_Little_Endian &&
                      std::is_same<std::false_type, std::integral_constant<bool, BigEndianDest>>::value )
                        )
            {
                /*
                char32_t cu1    = (code_unit & 0x000000FF);
                char32_t cu2    = (code_unit & 0x0000FF00);
                char32_t cu3    = (code_unit & 0x00FF0000);
                char32_t cu4    = (code_unit & 0xFF000000);

                code_unit       = ( (cu4 >> 24) | (cu3 >> 8) | (cu2 << 8) | (cu1 << 24) );
                */
                code_unit   = ( ( (code_unit & 0xFF000000) >> 24 ) |
                                ( (code_unit & 0x00FF0000) >>  8 ) |
                                ( (code_unit & 0x0000FF00) <<  8 ) |
                                ( (code_unit & 0x000000FF) << 24 ) );
            }

            output[ctr++]   = code_unit;
        }

        return Result(ctr, size / k_Convertion_Factor);
    }
}

}

#endif  //_UNICODE_HPP__
