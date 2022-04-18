#ifndef _UNICODE_HPP__
#define _UNICODE_HPP__

#include <algorithm>
#include <execution>
#include <iostream>

#include "util/helper_functions.hpp"

namespace util
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
    static void simplify(DestType* output, const int64_t size) noexcept;
    [[nodiscard]] static int64_t verify(DestType* output, const int64_t size) noexcept;
    static void nullify(DestType* output, const int64_t size) noexcept;
    static void modify(DestType* output, const int64_t size) noexcept;
    [[nodiscard]] static Result qualify(DestType* output, const int64_t size) noexcept;

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
    //std::for_each(input, input + size, [](const SrcType& v){std::cout << std::hex << v << " "; } );
    //std::cout << std::endl;

    magnify(output, input, size);
    simplify(output, size);

    int64_t invalid_index  = verify(output, size);

    //std::cout << std::dec << invalid_index << std::endl;

    nullify(output, invalid_index);
    modify(output, invalid_index);

    return qualify(output, invalid_index);

    /*
    auto [o_size, i_remain] = qualify(output, invalid_index);

    std::for_each(output, output + size * k_Convertion_Factor, [](const DestType& v){std::cout << std::hex << v << " "; } );
    std::cout << std::endl;

    std::cout << std::dec << o_size << "-" << i_remain << std::endl;

    return Result(o_size, i_remain);
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
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::magnify(DestType* output, const SrcType* input, const int64_t size) noexcept
{
    auto to_dest = [&output, &input](const SrcType& value)
        {
            if constexpr (std::is_same<char8_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value)
            {
                int64_t index   = &value - input;
                output[index]   = value;
            }

            if constexpr (std::is_same<char8_t,  DestType>::value && std::is_same<char16_t, SrcType>::value)
            {
                int64_t index           = (&value - input) * k_Convertion_Factor;

                // char8_t, char16_t, and char32_t are unsigned type right shift doesn't carry 1 from MSB
                // uint_least8_t, uint_least16_t, and uint_least32_t repectively

                if constexpr (Endian::k_Little_Endian)
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( (value & 0xFF00) >> 8 );
                        output[index + 1]   = ( (value & 0x00FF)      );
                        output[index + 2]   = 0;
                        output[index + 3]   = 0;
                    }
                    else
                    {
                        output[index]       = ( (value & 0x00FF)      );
                        output[index + 1]   = ( (value & 0xFF00) >> 8 );
                        output[index + 2]   = 0;
                        output[index + 3]   = 0;
                        //*(reinterpret_cast<char32_t*>(&output[index]))   = static_cast<char32_t>(value);
                    }
                }
                else
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( (value & 0x00FF)      );
                        output[index + 1]   = ( (value & 0xFF00) >> 8 );
                        output[index + 2]   = 0;
                        output[index + 3]   = 0;
                        //*(reinterpret_cast<char32_t*>(&output[index]))   = static_cast<char32_t>(value);
                    }
                    else
                    {
                        output[index]       = ( (value & 0xFF00) >> 8 );
                        output[index + 1]   = ( (value & 0x00FF)      );
                        output[index + 2]   = 0;
                        output[index + 3]   = 0;
                    }
                }
            }

            if constexpr (std::is_same<char8_t,  DestType>::value && std::is_same<char32_t, SrcType>::value)
            {
                int64_t index           = (&value - input) * k_Convertion_Factor;

                if constexpr (Endian::k_Little_Endian)
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( (value & 0xFF000000) >> 24 );
                        output[index + 1]   = ( (value & 0x00FF0000) >> 16 );
                        output[index + 2]   = ( (value & 0x0000FF00) >> 8  );
                        output[index + 3]   = ( (value & 0x000000FF)       );
                    }
                    else
                    {
                        *(reinterpret_cast<char32_t*>(&output[index]))   = static_cast<char32_t>(value);
                        //output[index]       = ( (value & 0x000000FF)       );
                        //output[index + 1]   = ( (value & 0x0000FF00) >> 8  );
                        //output[index + 2]   = ( (value & 0x00FF0000) >> 16 );
                        //output[index + 3]   = ( (value & 0xFF000000) >> 24 );
                    }
                }
                else
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        *(reinterpret_cast<char32_t*>(&output[index]))   = static_cast<char32_t>(value);
                        //output[index]       = ( (value & 0x000000FF)       );
                        //output[index + 1]   = ( (value & 0x0000FF00) >> 8  );
                        //output[index + 2]   = ( (value & 0x00FF0000) >> 16 );
                        //output[index + 3]   = ( (value & 0xFF000000) >> 24 );
                    }
                    else
                    {
                        output[index]       = ( (value & 0xFF000000) >> 24 );
                        output[index + 1]   = ( (value & 0x00FF0000) >> 16 );
                        output[index + 2]   = ( (value & 0x0000FF00) >> 8  );
                        output[index + 3]   = ( (value & 0x000000FF)       );
                    }
                }
            }

            if constexpr (std::is_same<char16_t, DestType>::value && std::is_same<char8_t,  SrcType>::value)
            {
                int64_t index = (&value - input) * k_Convertion_Factor;

                *(reinterpret_cast<char32_t*>(&output[index]))   = static_cast<char32_t>(value);
            }

            if constexpr (std::is_same<char16_t, DestType>::value && std::is_same<char16_t, SrcType>::value)
            {
                int64_t index   = &value - input;

                if constexpr (Endian::k_Little_Endian)
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( ((0x00FF & value) << 8) | ((0xFF00 & value) >> 8) );
                    }
                    else
                    {
                        output[index]       = value;
                    }
                }
                else
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = value;
                    }
                    else
                    {
                        output[index]       = ( ((0x00FF & value) << 8) | ((0xFF00 & value) >> 8) );
                    }
                }
            }

            if constexpr (std::is_same<char16_t, DestType>::value && std::is_same<char32_t, SrcType>::value)
            {
                int64_t index           = (&value - input) * k_Convertion_Factor;

                if constexpr (Endian::k_Little_Endian)
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24) );
                        output[index + 1]   = ( ((value & 0x000000FF) << 8) | ((value & 0x0000FF00) >> 8)  );
                    }
                    else
                    {
                        output[index]       = ( (value & 0x0000FFFF)         );
                        output[index + 1]   = ( ((value & 0xFFFF0000) >> 16) );
                        //*(reinterpret_cast<char32_t*>(&output[index]))   = static_cast<char32_t>(value);
                    }
                }
                else
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( (value & 0x0000FFFF)         );
                        output[index + 1]   = ( ((value & 0xFFFF0000) >> 16) );
                        //*(reinterpret_cast<char32_t*>(&output[index]))   = static_cast<char32_t>(value);
                    }
                    else
                    {
                        output[index]       = ( ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24) );
                        output[index + 1]   = ( ((value & 0x000000FF) << 8) | ((value & 0x0000FF00) >> 8)  );
                    }
                }
            }

            if constexpr (std::is_same<char32_t, DestType>::value && std::is_same<char8_t,  SrcType>::value)
            {
                int64_t index   = &value - input;
                output[index]   = static_cast<DestType>(value);
            }

            if constexpr (std::is_same<char32_t, DestType>::value && std::is_same<char16_t, SrcType>::value)
            {
                int64_t index   = &value - input;

                if constexpr (Endian::k_Little_Endian)
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( ((static_cast<DestType>(0x00FF & value)) << 8) |
                                                ((static_cast<DestType>(0xFF00 & value)) >> 8) );
                    }
                    else
                    {
                        output[index]       = static_cast<DestType>(value);
                    }
                }
                else
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = static_cast<DestType>(value);
                    }
                    else
                    {
                        output[index]       = ( ((static_cast<DestType>(0x00FF & value)) << 8) |
                                                ((static_cast<DestType>(0xFF00 & value)) >> 8) );
                    }
                }
            }

            if constexpr (std::is_same<char32_t, DestType>::value && std::is_same<char32_t, SrcType>::value)
            {
                int64_t index   = &value - input;

                if constexpr (Endian::k_Little_Endian)
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = ( ((0x000000FF & value) << 24) |
                                                ((0x0000FF00 & value) << 8 ) |
                                                ((0x00FF0000 & value) >> 8 ) |
                                                ((0xFF000000 & value) >> 24) );
                    }
                    else
                    {
                        output[index]       = value;
                    }
                }
                else
                {
                    if constexpr (std::is_same<std::true_type, std::integral_constant<bool, BigEndianSrc>>::value)
                    {
                        output[index]       = value;
                    }
                    else
                    {
                        output[index]       = ( ((0x000000FF & value) << 24) |
                                                ((0x0000FF00 & value) << 8 ) |
                                                ((0x00FF0000 & value) >> 8 ) |
                                                ((0xFF000000 & value) >> 24) );
                    }
                }
            }
        };

    std::for_each(std::execution::unseq, input, input + size, to_dest);

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
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::simplify(DestType* output, const int64_t size) noexcept
{
    if constexpr (
            (std::is_same<char16_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto u8_to_32  = [&output32_p, &size](const char32_t& value)
            {
                if ( ((value & 0x00000080) == 0x0)        ||
                     ((value & 0x000000C0) == 0x00000080) )
                    return;

                uint8_t num         = static_cast<uint8_t>(value & 0x000000FF);
                uint8_t num_bytes   = 0;

                while (num & 0x80)
                {
                    num_bytes++;
                    num = num << 1;
                }

                int64_t index       = &value - output32_p;

                // last incomplete element
                if (index + num_bytes > size)
                    return;

                uint8_t ctr = 0;
                char32_t cu = output32_p[index];
                while (++ctr < num_bytes)
                {
                    char32_t next_cu = output32_p[index + ctr];
                    cu = ( (cu << 8) | (next_cu & 0x000000FF) ); 
                }

                output32_p[index] = cu;

                return;
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size, u8_to_32);
    }
    else if constexpr (
            (std::is_same<char8_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto u16_to_32  = [&output32_p, &size](const char32_t& value)
            {
                if ((value & 0x0000FC00) != 0x0000D800)
                    return;

                int64_t index       = &value - output32_p;

                // last incomplete element or lower surrogate
                if (((value & 0x0000FC00) == 0x0000DC00) || (index + 1 == size))
                    return;

                output32_p[index]   = ( (value << 16) | (output32_p[index + 1] & 0x0000FFFF) );

                return;
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size, u16_to_32);
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
[[nodiscard]] int64_t UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::verify(DestType* output, const int64_t size) noexcept
{
    if constexpr (
            (std::is_same<char8_t,   DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        int64_t length  = size;

        if (length > 0 && (output[length - 1] & 0x80) == 0x0)
            return size;

        while (length > 0 && (output[length - 1] & 0xC0) == 0x80)
            length--;

        char8_t cu      = output[length - 1];
        uint8_t num_cus = 0;

        while ((cu & 0x80) != 0x0)
        {
            num_cus++;
            cu = (cu << 1);
        }

        return (size - length) == num_cus ? size : (length - 1);
    }
    else if constexpr (
            (std::is_same<char16_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char32_t,  DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);
        int64_t length          = size;

        if (length > 0 && (output32_p[length - 1] & 0x80) == 0x0)
            return size * k_Convertion_Factor;

        while (length > 0 && (output32_p[length - 1] & 0xC0) == 0x80)
            length--;

        char32_t cu     = output32_p[length - 1];
        uint8_t num_cus = 0;

        while ((cu & 0x80) != 0x0)
        {
            num_cus++;
            cu = (cu << 1);
        }

        return (size - length) == num_cus ? size * k_Convertion_Factor : (length -1) * k_Convertion_Factor;

    }
    else if constexpr (
            (std::is_same<char16_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        int64_t length = size;

        if (length > 0 && (output[length - 1] & 0xFC00) == 0xD800)
            length--;

        return length;
    }
    else if constexpr (
            (std::is_same<char8_t,   DestType>::value && std::is_same<char16_t, SrcType>::value) ||
            (std::is_same<char32_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);
        int64_t length          = size;

        if (length > 0 && (output32_p[length - 1] & 0xFC00) == 0xD800)
            length--;

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
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::nullify(DestType* output, const int64_t size) noexcept
{
    if constexpr (
            (std::is_same<char16_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto clear_u8  = [&output32_p](const char32_t& value)
            {
                if ((value & 0xFFFFFFC0) == 0x00000080)
                {
                    int64_t index       = &value - output32_p;
                    output32_p[index]   = 0xFFFFFFFF;
                }

                return;
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size / k_Convertion_Factor, clear_u8);
    }
    else if constexpr (
            (std::is_same<char8_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto clear_u16  = [&output32_p](const char32_t& value)
            {
                if ((value & 0xFFFFFC00) == 0x0000DC00)
                {
                    int64_t index       = &value - output32_p;
                    output32_p[index]   = 0xFFFFFFFF;
                }

                return;
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size / k_Convertion_Factor, clear_u16);
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
void UniFy<DestType, SrcType, BigEndianDest, BigEndianSrc, t_dest_ptr>::modify(DestType* output, const int64_t size) noexcept
{
    if constexpr (
            (std::is_same<char16_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char8_t,  SrcType>::value) )
    {
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
                }
                else
                {
                    output32_p[index]   = code_point;
                }

                return;
            };

        std::for_each(std::execution::unseq, output32_p, output32_p + size / k_Convertion_Factor, decode_u8);
    }
    else if constexpr (
            (std::is_same<char8_t,  DestType>::value && std::is_same<char16_t, SrcType>::value) ||
            (std::is_same<char32_t, DestType>::value && std::is_same<char16_t, SrcType>::value) )
    {
        char32_t* output32_p    = reinterpret_cast<char32_t*>(output);

        auto decode_u16 = [&output32_p](const char32_t& value)
            {
                if (value == 0xFFFFFFFF || value < 0x00010000)
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
                    ( Endian::k_Little_Endian &&
                      std::is_same<std::true_type,  std::integral_constant<bool, BigEndianDest>>::value ) ||
                    ( !Endian::k_Little_Endian &&
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
                    ( Endian::k_Little_Endian &&
                      std::is_same<std::true_type,  std::integral_constant<bool, BigEndianDest>>::value ) ||
                    ( !Endian::k_Little_Endian &&
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

            if (code_point < 0x00000080)
            {
                output[ctr++]   = static_cast<DestType>(code_point);
                continue;
            }

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

            for (int i = 1; i >= 0; --i)
            {
                char16_t cu         = ( (code_point & (0xFFFF << (i * 16))) >> (i * 16) );

                if constexpr (
                        ( Endian::k_Little_Endian &&
                          std::is_same<std::true_type,  std::integral_constant<bool, BigEndianDest>>::value ) ||
                        ( !Endian::k_Little_Endian &&
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
                    ( Endian::k_Little_Endian &&
                      std::is_same<std::true_type, std::integral_constant<bool, BigEndianDest>>::value  ) ||
                    ( !Endian::k_Little_Endian &&
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
