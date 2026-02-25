#pragma once

#include "snmalloc/stl/array.h"

#include <stddef.h>
#include <stdint.h>

namespace snmalloc
{
  /**
   * Weak AVL tree implementation using 2 bits per node to encode whether each
   * child has rank difference 2.
   *
   * The representation must provide:
   * - types `Handle`, `Contents`
   * - `get(Handle)`, `set(Handle, Contents)`
   * - `ref(bool is_left, Contents)`
   * - `get_bits(Contents)`, `set_bits(Contents, uint8_t)`
   * - `compare(Contents, Contents)`, `equal(Contents, Contents)`
   * - constants `null` and `root`
   */
  template<typename Rep, bool run_checks = Debug, bool TRACE = false>
  class RBTree
  {
    using H = typename Rep::Handle;
    using K = typename Rep::Contents;
    using RootStorage =
      stl::remove_const_t<stl::remove_reference_t<decltype(Rep::root)>>;

    RootStorage root{Rep::root};

    static constexpr bool Left = false;
    static constexpr bool Right = true;

    static constexpr uint8_t LeftFlagBit = 0b01;
    static constexpr uint8_t RightFlagBit = 0b10;
    static constexpr uint8_t FlagsMask = 0b11;

    struct FixupSite
    {
      K parent;
      bool is_right;
    };

    class RBPath
    {
      friend class RBTree;

      K parent{Rep::null};
      K curr{Rep::null};
      bool dir{Left};
    };

    static constexpr bool use_checks = run_checks;

    H root_ref()
    {
      return H{&root};
    }

    K get_root() const
    {
      return Rep::get(H{const_cast<RootStorage*>(&root)});
    }

    void set_root(K n)
    {
      set(root_ref(), n);
    }

    static K get(H p)
    {
      return Rep::get(p);
    }

    static void set(H p, K v)
    {
      Rep::set(p, v);
    }

    static H child_ref(K n, bool dir)
    {
      // Rep uses true for left, false for right.
      return Rep::ref(!dir, n);
    }

    static K child(K n, bool dir)
    {
      return get(child_ref(n, dir));
    }

    static void set_child(K n, bool dir, K v)
    {
      set(child_ref(n, dir), v);
    }

    K parent(K n) const
    {
      if (is_null(n))
        return Rep::null;

      K p = Rep::null;
      K cur = get_root();
      while (!is_null(cur) && !Rep::equal(cur, n))
      {
        p = cur;
        bool dir = Rep::compare(cur, n) ? Left : Right;
        cur = child(cur, dir);
      }
      return is_null(cur) ? Rep::null : p;
    }

    static void set_parent(K n, K p)
    {
      UNUSED(n, p);
    }

    static bool is_null(K n)
    {
      return Rep::equal(n, Rep::null);
    }

    static uint8_t flags(K n)
    {
      if (is_null(n))
        return 0;
      return Rep::get_bits(n) & FlagsMask;
    }

    static void set_flags(K n, uint8_t bits)
    {
      if (!is_null(n))
        Rep::set_bits(n, bits & FlagsMask);
    }

    static void clear_flags(K n)
    {
      set_flags(n, 0);
    }

    static bool has_rank_diff_2(K n, bool dir)
    {
      auto bit = dir ? RightFlagBit : LeftFlagBit;
      return (flags(n) & bit) != 0;
    }

    static void toggle_rank_diff_2(K n, bool dir)
    {
      auto bit = dir ? RightFlagBit : LeftFlagBit;
      set_flags(n, static_cast<uint8_t>(flags(n) ^ bit));
    }

    static bool is_leaf(K n)
    {
      return is_null(child(n, Left)) && is_null(child(n, Right));
    }

    static bool is_right_child(K p, K n)
    {
      return Rep::equal(child(p, Right), n);
    }

    K rotate(K node, bool dir)
    {
      K pivot = child(node, dir);
      K grandchild = child(pivot, !dir);

      set_child(node, dir, grandchild);
      if (!is_null(grandchild))
        set_parent(grandchild, node);

      K p = parent(node);
      set_parent(pivot, p);

      if (is_null(p))
      {
        set_root(pivot);
      }
      else
      {
        bool node_is_right = is_right_child(p, node);
        set_child(p, node_is_right, pivot);
      }

      set_child(pivot, !dir, node);
      set_parent(node, pivot);
      return pivot;
    }

    K find_node(K value) const
    {
      K cursor = get_root();
      while (!is_null(cursor))
      {
        if (Rep::equal(cursor, value))
          return cursor;

        bool dir = Rep::compare(cursor, value) ? Left : Right;
        cursor = child(cursor, dir);
      }
      return Rep::null;
    }

    FixupSite unlink_node(K node)
    {
      bool has_left = !is_null(child(node, Left));
      bool has_right = !is_null(child(node, Right));

      if (!has_left && !has_right)
      {
        K p = parent(node);
        if (is_null(p))
        {
          set_root(Rep::null);
          return {Rep::null, false};
        }

        FixupSite site = {p, is_right_child(p, node)};
        set_child(site.parent, site.is_right, Rep::null);
        return site;
      }

      if (has_left != has_right)
      {
        K child_node = has_right ? child(node, Right) : child(node, Left);
        K p = parent(node);
        if (is_null(p))
        {
          set_root(child_node);
          set_parent(child_node, Rep::null);
          return {Rep::null, false};
        }

        FixupSite site = {p, is_right_child(p, node)};
        set_child(site.parent, site.is_right, child_node);
        set_parent(child_node, site.parent);
        return site;
      }

      K succ = child(node, Right);
      while (!is_null(child(succ, Left)))
        succ = child(succ, Left);

      K succ_parent = parent(succ);
      bool succ_was_right = is_right_child(succ_parent, succ);
      K succ_right = child(succ, Right);

      FixupSite site = {succ_parent, succ_was_right};
      set_child(succ_parent, succ_was_right, succ_right);
      if (!is_null(succ_right))
        set_parent(succ_right, succ_parent);

      K p = parent(node);
      set_parent(succ, p);
      set_flags(succ, flags(node));

      set_child(succ, Left, child(node, Left));
      set_child(succ, Right, child(node, Right));
      if (!is_null(child(succ, Left)))
        set_parent(child(succ, Left), succ);
      if (!is_null(child(succ, Right)))
        set_parent(child(succ, Right), succ);

      if (!is_null(p))
      {
        bool node_was_right = is_right_child(p, node);
        set_child(p, node_was_right, succ);
      }
      else
      {
        set_root(succ);
      }

      if (Rep::equal(site.parent, node))
        site.parent = succ;

      return site;
    }

    void erase_node(K node)
    {
      auto [cursor, is_right] = unlink_node(node);

      set_child(node, Left, Rep::null);
      set_child(node, Right, Rep::null);
      set_parent(node, Rep::null);
      clear_flags(node);

      while (!is_null(cursor))
      {
        if (!has_rank_diff_2(cursor, is_right))
        {
          toggle_rank_diff_2(cursor, is_right);
          if (flags(cursor) == 0b11 && is_leaf(cursor))
          {
            clear_flags(cursor);
            K p = parent(cursor);
            if (!is_null(p))
              is_right = is_right_child(p, cursor);
            cursor = p;
            continue;
          }
          break;
        }

        if (has_rank_diff_2(cursor, !is_right))
        {
          toggle_rank_diff_2(cursor, !is_right);
          K p = parent(cursor);
          if (!is_null(p))
            is_right = is_right_child(p, cursor);
          cursor = p;
          continue;
        }

        K sibling = child(cursor, !is_right);
        if constexpr (use_checks)
          SNMALLOC_ASSERT(!is_null(sibling));

        if (flags(sibling) == 0b11)
        {
          clear_flags(sibling);
          K p = parent(cursor);
          if (!is_null(p))
            is_right = is_right_child(p, cursor);
          cursor = p;
          continue;
        }

        bool sibling_is_right = !is_right;
        if (!has_rank_diff_2(sibling, sibling_is_right))
        {
          K new_subroot = rotate(cursor, sibling_is_right);
          UNUSED(new_subroot);

          bool sibling_alter_child_has_rank_diff_2 =
            has_rank_diff_2(new_subroot, !sibling_is_right);

          clear_flags(new_subroot);
          toggle_rank_diff_2(new_subroot, sibling_is_right);

          if (sibling_alter_child_has_rank_diff_2)
          {
            if (is_leaf(cursor))
            {
              clear_flags(cursor);
              toggle_rank_diff_2(new_subroot, !sibling_is_right);
            }
            else
            {
              toggle_rank_diff_2(cursor, sibling_is_right);
            }
          }
          break;
        }

        K target_child = rotate(sibling, !sibling_is_right);
        uint8_t old_flags = flags(target_child);
        K new_subroot = rotate(cursor, sibling_is_right);
        UNUSED(new_subroot);
        if constexpr (use_checks)
          SNMALLOC_ASSERT(Rep::equal(new_subroot, target_child));

        set_flags(target_child, 0b11);
        clear_flags(cursor);
        clear_flags(sibling);

        K dst_left = sibling_is_right ? cursor : sibling;
        K dst_right = sibling_is_right ? sibling : cursor;
        if ((old_flags & LeftFlagBit) != 0)
          toggle_rank_diff_2(dst_left, Right);
        if ((old_flags & RightFlagBit) != 0)
          toggle_rank_diff_2(dst_right, Left);
        break;
      }
    }

    void insert_known_absent(K value, K parent_node, bool dir)
    {
      set_parent(value, parent_node);
      set_child(value, Left, Rep::null);
      set_child(value, Right, Rep::null);
      clear_flags(value);

      if (is_null(parent_node))
      {
        set_root(value);
        return;
      }

      set_child(parent_node, dir, value);

      K node = value;
      K p = parent_node;
      while (!is_null(p))
      {
        bool is_right = is_right_child(p, node);

        if (has_rank_diff_2(p, is_right))
        {
          toggle_rank_diff_2(p, is_right);
          break;
        }

        bool sibling_has_rank_diff_2 = has_rank_diff_2(p, !is_right);
        if (!sibling_has_rank_diff_2)
        {
          toggle_rank_diff_2(p, !is_right);
          node = p;
          p = parent(node);
          continue;
        }

        if (has_rank_diff_2(node, !is_right))
        {
          K new_subroot = rotate(p, is_right);
          clear_flags(new_subroot);
          clear_flags(p);
          break;
        }

        K subroot1 = rotate(node, !is_right);
        K subroot2 = rotate(p, is_right);
        UNUSED(subroot2);
        if constexpr (use_checks)
          SNMALLOC_ASSERT(Rep::equal(subroot1, subroot2));

        uint8_t old_flags = flags(subroot1);
        clear_flags(node);
        clear_flags(p);
        clear_flags(subroot1);

        K dst_left = is_right ? p : node;
        K dst_right = is_right ? node : p;
        if ((old_flags & LeftFlagBit) != 0)
          toggle_rank_diff_2(dst_left, Right);
        if ((old_flags & RightFlagBit) != 0)
          toggle_rank_diff_2(dst_right, Left);
        break;
      }
    }

  public:
    constexpr RBTree() = default;

    bool is_empty()
    {
      return is_null(get_root());
    }

    bool insert_elem(K value)
    {
      auto path = get_root_path();
      if (find(path, value))
        return false;
      insert_path(path, value);
      return true;
    }

    bool remove_elem(K value)
    {
      auto path = get_root_path();
      if (!find(path, value))
        return false;
      remove_path(path);
      return true;
    }

    K remove_min()
    {
      K cursor = get_root();
      if (is_null(cursor))
        return Rep::null;

      while (!is_null(child(cursor, Left)))
        cursor = child(cursor, Left);

      erase_node(cursor);
      return cursor;
    }

    bool find(RBPath& path, K value)
    {
      K parent_node = Rep::null;
      K cursor = get_root();
      bool dir = Left;

      while (!is_null(cursor))
      {
        if (Rep::equal(cursor, value))
        {
          path.parent = parent_node;
          path.curr = cursor;
          path.dir = dir;
          return true;
        }

        parent_node = cursor;
        dir = Rep::compare(cursor, value) ? Left : Right;
        cursor = child(cursor, dir);
      }

      path.parent = parent_node;
      path.curr = Rep::null;
      path.dir = dir;
      return false;
    }

    bool remove_path(RBPath& path)
    {
      if (is_null(path.curr))
        return false;

      erase_node(path.curr);
      return true;
    }

    void insert_path(RBPath& path, K value)
    {
      if constexpr (use_checks)
        SNMALLOC_ASSERT(is_null(path.curr));

      insert_known_absent(value, path.parent, path.dir);
      path.curr = value;
    }

    RBPath get_root_path()
    {
      return RBPath{};
    }
  };
} // namespace snmalloc
