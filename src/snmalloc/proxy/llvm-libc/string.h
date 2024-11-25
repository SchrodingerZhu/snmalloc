#pragma once

#include "src/__support/macros/attributes.h"
#ifndef LIBC_NAMESPACE
#  define LIBC_NAMESPACE __llvm_libc
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/memory_utils/inline_memmove.h"
#include "src/string/memory_utils/inline_memset.h"
#include "src/string/string_utils.h"
#pragma GCC diagnostic pop

namespace cpp
{
  LIBC_INLINE void*
  memcpy(void* __restrict dst, const void* __restrict src, size_t n)
  {
    LIBC_NAMESPACE::inline_memcpy(dst, src, n);
    return dst;
  }

  LIBC_INLINE void* memmove(void* dst, const void* src, size_t n)
  {
    LIBC_NAMESPACE::inline_memmove(dst, src, n);
    return dst;
  }

  LIBC_INLINE void* memset(void* dst, uint8_t c, size_t n)
  {
    LIBC_NAMESPACE::inline_memset(dst, c, n);
    return dst;
  }

  LIBC_INLINE size_t strlen(const char* src)
  {
    return LIBC_NAMESPACE::internal::string_length(src);
  }

} // namespace cpp