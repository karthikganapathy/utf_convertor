#include "karthik.h"

ptrdiff_t
unifyUtf32(char8_t const* src, size_t srcBytes, char32_t* dst)
{
    using   UnicodeLL       = utf::UniFy<char32_t, char8_t, false, false>;

    auto    [o_size, i_rem] = UnicodeLL::transcode(dst, src, srcBytes);
    char32_t*   d           = dst + o_size;

    return d - dst;
}

ptrdiff_t
unifyUtf16(char8_t const* src, size_t srcBytes, char16_t* dst)
{
    using   UnicodeLL       = utf::UniFy<char16_t, char8_t, false, false>;

    auto    [o_size, i_rem] = UnicodeLL::transcode(dst, src, srcBytes);
    char16_t*   d           = dst + o_size;

    return d - dst;
}
