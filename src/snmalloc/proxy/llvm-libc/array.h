#pragma once

#ifndef LIBC_NAMESPACE
#  define LIBC_NAMESPACE __llvm_libc
#endif

#include "src/__support/CPP/iterator.h"

namespace cpp
{
  template<class T, __SIZE_TYPE__ N>
  struct array
  {
    T Data[N];
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = LIBC_NAMESPACE::cpp::reverse_iterator<iterator>;
    using const_reverse_iterator =
      LIBC_NAMESPACE::cpp::reverse_iterator<const_iterator>;
    using size_type = __SIZE_TYPE__;

    constexpr T* data()
    {
      return Data;
    }

    constexpr const T* data() const
    {
      return Data;
    }

    constexpr T& front()
    {
      return Data[0];
    }

    constexpr const T& front() const
    {
      return Data[0];
    }

    constexpr T& back()
    {
      return Data[N - 1];
    }

    constexpr const T& back() const
    {
      return Data[N - 1];
    }

    constexpr T& operator[](size_type Index)
    {
      return Data[Index];
    }

    constexpr const T& operator[](size_type Index) const
    {
      return Data[Index];
    }

    [[nodiscard]] constexpr size_type size() const
    {
      return N;
    }

    [[nodiscard]] constexpr bool empty() const
    {
      return N == 0;
    }

    constexpr iterator begin()
    {
      return Data;
    }

    constexpr const_iterator begin() const
    {
      return Data;
    }

    constexpr const_iterator cbegin() const
    {
      return begin();
    }

    constexpr iterator end()
    {
      return Data + N;
    }

    constexpr const_iterator end() const
    {
      return Data + N;
    }

    constexpr const_iterator cend() const
    {
      return end();
    }

    constexpr reverse_iterator rbegin()
    {
      return reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rbegin() const
    {
      return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator crbegin() const
    {
      return rbegin();
    }

    constexpr reverse_iterator rend()
    {
      return reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator rend() const
    {
      return const_reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crend() const
    {
      return rend();
    }
  };

  template<typename T, __SIZE_TYPE__ N>
  T* begin(T (&array)[N])
  {
    return array;
  }

  template<typename T, __SIZE_TYPE__ N>
  T* end(T (&array)[N])
  {
    return array + N;
  }

  // deduction guide
  template<class T, __SIZE_TYPE__ N>
  array(T (&)[N]) -> array<T, N>;

} // namespace cpp
