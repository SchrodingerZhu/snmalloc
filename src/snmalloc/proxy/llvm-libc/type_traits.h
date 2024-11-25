#pragma once

#ifndef LIBC_NAMESPACE
#  define LIBC_NAMESPACE __llvm_libc
#endif

#include "src/__support/CPP/type_traits.h"

namespace cpp
{
  using namespace LIBC_NAMESPACE::cpp;

  template<class T>
  struct is_trivially_default_constructible
  {
    static constexpr bool value = __is_trivially_constructible(T);
  };

  template<class T>
  constexpr bool is_trivially_default_constructible_v =
    is_trivially_default_constructible<T>::value;
} // namespace cpp
