#pragma once

namespace cpp
{
  enum class memory_order : int
  {
    RELAXED = __ATOMIC_RELAXED,
    CONSUME = __ATOMIC_CONSUME,
    ACQUIRE = __ATOMIC_ACQUIRE,
    RELEASE = __ATOMIC_RELEASE,
    ACQ_REL = __ATOMIC_ACQ_REL,
    SEQ_CST = __ATOMIC_SEQ_CST
  };

  constexpr memory_order memory_order_relaxed = memory_order::RELAXED;
  constexpr memory_order memory_order_consume = memory_order::CONSUME;
  constexpr memory_order memory_order_acquire = memory_order::ACQUIRE;
  constexpr memory_order memory_order_release = memory_order::RELEASE;
  constexpr memory_order memory_order_acq_rel = memory_order::ACQ_REL;
  constexpr memory_order memory_order_seq_cst = memory_order::SEQ_CST;

  template<typename T>
  struct atomic
  {
  private:
    // The value stored should be appropriately aligned so that
    // hardware instructions used to perform atomic operations work
    // correctly.
    static constexpr int ALIGNMENT = sizeof(T) > alignof(T) ? sizeof(T) :
                                                              alignof(T);

  public:
    using value_type = T;

    // We keep the internal value public so that it can be addressable.
    // This is useful in places like the Linux futex operations where
    // we need pointers to the memory of the atomic values. Load and store
    // operations should be performed using the atomic methods however.
    alignas(ALIGNMENT) value_type val;

    constexpr atomic() = default;

    // Intializes the value without using atomic operations.
    constexpr atomic(value_type v) : val(v) {}

    atomic(const atomic&) = delete;
    atomic& operator=(const atomic&) = delete;

    // atomic load.
    operator T()
    {
      return load();
    }

    T load(memory_order mem_ord = memory_order::SEQ_CST)
    {
      T result;
      __atomic_load(&val, &result, static_cast<int>(mem_ord));
      return result;
    }

    // atomic store.
    atomic<T>& operator=(T rhs)
    {
      store(rhs);
      return *this;
    }

    void store(T rhs, memory_order mem_ord = memory_order::SEQ_CST)
    {
      __atomic_store(&val, &rhs, static_cast<int>(mem_ord));
    }

    // atomic compare exchange
    bool compare_exchange_strong(
      T& expected, T desired, memory_order mem_ord = memory_order::SEQ_CST)
    {
      return __atomic_compare_exchange(
        &val, &expected, &desired, false, int(mem_ord), int(mem_ord));
    }

    // atomic compare exchange (separate success and failure memory orders)
    bool compare_exchange_strong(
      T& expected,
      T desired,
      memory_order success_order,
      memory_order failure_order)
    {
      return __atomic_compare_exchange(
        &val,
        &expected,
        &desired,
        false,
        static_cast<int>(success_order),
        static_cast<int>(failure_order));
    }

    // atomic compare exchange (weak version)
    bool compare_exchange_weak(
      T& expected, T desired, memory_order mem_ord = memory_order::SEQ_CST)
    {
      return __atomic_compare_exchange(
        &val,
        &expected,
        &desired,
        true,
        static_cast<int>(mem_ord),
        static_cast<int>(mem_ord));
    }

    // atomic compare exchange (weak version with separate success and failure
    // memory orders)
    bool compare_exchange_weak(
      T& expected,
      T desired,
      memory_order success_order,
      memory_order failure_order)
    {
      return __atomic_compare_exchange(
        &val,
        &expected,
        &desired,
        true,
        static_cast<int>(success_order),
        static_cast<int>(failure_order));
    }

    T exchange(T desired, memory_order mem_ord = memory_order::SEQ_CST)
    {
      T result;
      __atomic_exchange(&val, &desired, &result, static_cast<int>(mem_ord));
      return result;
    }

    T fetch_add(T increment, memory_order mem_ord = memory_order::SEQ_CST)
    {
      return __atomic_fetch_add(&val, increment, static_cast<int>(mem_ord));
    }

    T fetch_or(T mask, memory_order mem_ord = memory_order::SEQ_CST)
    {
      return __atomic_fetch_or(&val, mask, static_cast<int>(mem_ord));
    }

    T fetch_and(T mask, memory_order mem_ord = memory_order::SEQ_CST)
    {
      return __atomic_fetch_and(&val, mask, static_cast<int>(mem_ord));
    }

    T fetch_sub(T decrement, memory_order mem_ord = memory_order::SEQ_CST)
    {
      return __atomic_fetch_sub(&val, decrement, static_cast<int>(mem_ord));
    }

    // Set the value without using an atomic operation. This is useful
    // in initializing atomic values without a constructor.
    void set(T rhs)
    {
      val = rhs;
    }

    T operator++()
    {
      return __atomic_add_fetch(
        &val, T{1}, static_cast<int>(memory_order::SEQ_CST));
    }

    const T operator++(int)
    {
      return fetch_add(T{1});
    }

    T operator-=(T rhs)
    {
      return __atomic_sub_fetch(
        &val, rhs, static_cast<int>(memory_order::SEQ_CST));
    }

    T operator+=(T rhs)
    {
      return __atomic_add_fetch(
        &val, rhs, static_cast<int>(memory_order::SEQ_CST));
    }

    T operator--()
    {
      return __atomic_sub_fetch(
        &val, T{1}, static_cast<int>(memory_order::SEQ_CST));
    }

    const T operator--(int)
    {
      return fetch_sub(T{1});
    }
  };

  using atomic_bool = atomic<bool>;
} // namespace cpp

#undef __cpp_lib_atomic_ref
