#include <cstdint>
#include <cstddef>

//- Just declarations.
//
ptrdiff_t   toUtf32(char8_t const* src, size_t srcBytes, char32_t* dst);
ptrdiff_t   toUtf16(char8_t const* src, size_t srcBytes, char16_t* dst);
