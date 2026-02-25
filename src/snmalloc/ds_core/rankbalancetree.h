#pragma once

#ifndef SNMALLOC_RBTREE_VARIANT
/**
 * Tree backend selection:
 * 0 = Red-Black tree (existing implementation)
 * 1 = Weak AVL tree with 2-bit rank-difference flags
 * 2 = Weak AVL tree with 1-bit rank parity
 */
#  define SNMALLOC_RBTREE_VARIANT 0
#endif

#if SNMALLOC_RBTREE_VARIANT == 0
#  include "redblacktree.h"
#elif SNMALLOC_RBTREE_VARIANT == 1
#  include "wavltree2.h"
#elif SNMALLOC_RBTREE_VARIANT == 2
#  include "wavltree1.h"
#else
#  error "Unsupported SNMALLOC_RBTREE_VARIANT value"
#endif
