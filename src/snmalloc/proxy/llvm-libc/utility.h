#pragma once

#ifndef LIBC_NAMESPACE
#  define LIBC_NAMESPACE __llvm_libc
#endif

#include "src/__support/CPP/utility.h"

namespace cpp
{
  using namespace LIBC_NAMESPACE::cpp;

  template<typename T, typename U = T>
  LIBC_INLINE T exchange(T& a, U&& b)
  {
    T old = LIBC_NAMESPACE::cpp::move(a);
    a = LIBC_NAMESPACE::cpp::forward<U>(b);
    return old;
  }

  template <typename A, typename B>
  struct pair {
    A first;
    B second;
  };
} // namespace cpp