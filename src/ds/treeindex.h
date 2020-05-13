#include "../aal/aal.h"

#include <atomic>
#include <array>

namespace snmalloc
{
  /**
   * This class provides an index of size total_entries.
   *
   * By specifying a way to allocated blocks, `alloc_block` and the block size
   * in bytes, `block_size` the index is made into a tree. `get` is wait-free
   * and has no conditionals, `set` can block if two threads are attempting to
   * add a sub-node in the tree at the same time.
   */
  template<
    typename T,
    size_t total_entries,
    void* (*alloc_block)() = nullptr,
    size_t block_size_template = 0>
  class TreeIndex
  {
    static constexpr size_t block_size = block_size_template == 0 ?
      total_entries * sizeof(T) :
      block_size_template;

    static_assert(
      (block_size_template == 0) == (alloc_block == nullptr),
      "Must set alloc_block and block_size_template parameter");

    static_assert(
      block_size >= sizeof(uintptr_t) * 2,
      "Block must contain at least two pointers.");
    static_assert(block_size >= sizeof(T), "Block must contain at least one T");

  public:
    static constexpr bool is_leaf = (total_entries * sizeof(T)) <= block_size;

    /**
     * Calculate the entries that are stored at this level of the tree.
     *
     * `TT` is used to allow the change of representation from T at the bottom
     *level to pointers at higher levels. `entries` is used to say how many
     *entries are required at this level.
     **/
    template<typename TT, size_t entries>
    static constexpr size_t calc_entries()
    {
      if constexpr (entries * sizeof(TT) <= block_size)
      {
        return entries;
      }
      else
      {
        constexpr size_t next = (entries * sizeof(TT)) / block_size;
        return calc_entries<void*, next>();
      }
    }

    /// The number of elements at this level of the tree.
    static constexpr size_t entries = calc_entries<T, total_entries>();
    /// The number of entries each sub entry should contain.
    static constexpr size_t sub_entries = total_entries / entries;

    /**
     * Type for the next level in the tree. If the current level is a leaf
     * then this is not used.
     */
    using SubT = std::conditional_t<
      is_leaf,
      void*,
      TreeIndex<T, sub_entries, alloc_block, block_size>>;

    /**
     * Type of entries in the tree that point to lower levels.
     * If this level is a leaf, then this type is not used.
     */
    struct Ptr
    {
      SubT* value;

      constexpr Ptr() noexcept : value(is_leaf ? nullptr : original()) {}

      Ptr(SubT* v) noexcept : value(v) {}
    };

    // Type of entries for this level of the tree
    using Entries = std::conditional_t<is_leaf, T, Ptr>;

#if !defined(_MSC_VER) || defined(__clang__)
    // The address used for default address for this level in the tree.
    inline static SubT original_block{};
#endif

    constexpr static SubT* original()
    {
#if defined(_MSC_VER) && !defined(__clang__)
      // The address used for default address for this level in the tree.
      // MSVC does not support the `inline static` of the self type.
      constexpr static SubT original_block{};
#endif
      if constexpr (is_leaf)
      {
        return nullptr;
      }
      else
      {
        return const_cast<SubT*>(&original_block);
      }
    };

#if !defined(_MSC_VER) || defined(__clang__)
    // The address used for the lock at for this level in the tree.
    inline static SubT lock_block{};
#endif
    constexpr static SubT* lock()
    {
#if defined(_MSC_VER) && !defined(__clang__)
      // The address used for the lock at for this level in the tree.
      // MSVC does not support the `inline static` of the self type.
      constexpr static SubT lock_block{};
#endif
      if constexpr (is_leaf)
      {
        return nullptr;
      }
      else
      {
        return const_cast<SubT*>(&lock_block);
      }
    };

    constexpr TreeIndex() noexcept : array() {}

    /// Get element at this index.
    T get(size_t index)
    {
      if constexpr (is_leaf)
      {
        return array[index].load(std::memory_order_relaxed);
      }
      else
      {
        return (array[index / sub_entries]
                  .load(std::memory_order_relaxed)
                  .value)
          ->get(index % sub_entries);
      }
    }

    /// Set element at this index.
    void set(size_t index, T v)
    {
      if constexpr (is_leaf)
      {
        array[index] = v;
      }
      else
      {
        auto next =
          array[index / sub_entries].load(std::memory_order_relaxed).value;
        if ((next != original()) && (next != lock()))
        {
          next->set(index % sub_entries, v);
          return;
        }
        set_slow(index, v);
      }
    }

    /**
     * Returns the pointer to the leaf entry associated with this index.
     * If the entry was not already in the tree creates a unique entry
     * for it.
     */
    std::atomic<T>* get_addr(size_t index)
    {
      if constexpr (is_leaf)
      {
        return &(array[index / sub_entries]);
      }
      else
      {
        auto expected = Ptr(original());
        if (array[index / sub_entries].compare_exchange_strong(
              expected, Ptr(lock())))
        {
          // Allocate new TreeIndex
          void* new_block = alloc_block();
          // Initialise the block
          auto new_block_typed = new (new_block) SubT();
          // Unlock TreeIndex
          array[index / sub_entries].store(
            Ptr(new_block_typed), std::memory_order_relaxed);
        }
        while (
          array[index / sub_entries].load(std::memory_order_relaxed).value ==
          lock())
        {
          Aal::pause();
        }

        return array[index / sub_entries]
          .load(std::memory_order_relaxed)
          .value->get_addr(index % sub_entries);
      }
    }

  private:
    void set_slow(size_t index, T v)
    {
      auto p = get_addr(index);
      p->store(v, std::memory_order_relaxed);
    }

    /**
     *  Data stored for this level of the tree.
     */
    std::array<std::atomic<Entries>, entries> array;
  };
}