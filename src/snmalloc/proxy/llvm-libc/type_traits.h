#pragma once

#include "src/__support/CPP/type_traits.h"

#include <type_traits>

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
