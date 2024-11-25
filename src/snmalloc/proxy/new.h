#pragma once

#include "snmalloc/ds_core/defines.h"
#include "snmalloc/proxy/size_t.h"

namespace snmalloc {
struct inplace_token_t {};
constexpr inline inplace_token_t inplace_token{};
} // namespace snmalloc

ALWAYSINLINE inline void* operator new(cpp::size_t, void* ptr, snmalloc::inplace_token_t) noexcept
{
  return ptr;
}

ALWAYSINLINE inline void* operator new[](cpp::size_t, void* ptr, snmalloc::inplace_token_t) noexcept
{
  return ptr;
}