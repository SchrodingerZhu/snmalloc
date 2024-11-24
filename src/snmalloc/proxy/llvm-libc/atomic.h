#pragma once

#include "src/__support/CPP/atomic.h"

namespace cpp
{
  template<class T>
  using atomic = LIBC_NAMESPACE::cpp::Atomic<T>;

  constexpr auto memory_order_relaxed =
    LIBC_NAMESPACE::cpp::MemoryOrder::RELAXED;
  constexpr auto memory_order_consume =
    LIBC_NAMESPACE::cpp::MemoryOrder::CONSUME;
  constexpr auto memory_order_acquire =
    LIBC_NAMESPACE::cpp::MemoryOrder::ACQUIRE;
  constexpr auto memory_order_release =
    LIBC_NAMESPACE::cpp::MemoryOrder::RELEASE;
  constexpr auto memory_order_acq_rel =
    LIBC_NAMESPACE::cpp::MemoryOrder::ACQ_REL;
  constexpr auto memory_order_seq_cst =
    LIBC_NAMESPACE::cpp::MemoryOrder::SEQ_CST;

  using atomic_bool = atomic<bool>;
  using memory_order = LIBC_NAMESPACE::cpp::MemoryOrder;
} // namespace cpp

#undef __cpp_lib_atomic_ref
