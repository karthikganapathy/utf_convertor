#ifndef _HELPER_FUNCTIONS_HPP__
#define _HELPER_FUNCTIONS_HPP__

#define LIKELY(expr)    (__builtin_expect(!!(expr), 1))
#define UNLIKELY(expr)  (__builtin_expect(!!(expr), 0))

namespace util
{
    namespace Endian
    {
    union EndianCheck
    {
        uint32_t    number_;
        uint8_t     bytes_[4];
    };

    static constexpr EndianCheck endian {0xABCDEF01};

    //static bool k_Little_Endian   = endian.bytes_[0] == (endian.number_ & 0x000000FF) && endian.bytes_[0] == 0x01; 
    static constexpr bool k_Little_Endian   = true;
    }

    template<typename EnumType>
    constexpr auto to_index(EnumType enum_value) noexcept
    {
        return static_cast<std::underlying_type_t<EnumType>>(enum_value);
    }
}

#endif // _HELPER_FUNCTIONS_HPP__
