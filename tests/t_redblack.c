/*
** Copyright (C) 2004 by Kevin L. Mitchell <klmitch@mit.edu>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the Free
** Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA
**
** @(#)$Id$
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "test-harness.h"

#include "dbprim.h"
#include "dbprim_int.h"

RCSTAG("@(#)$Id$");

TEST_PROG(t_redblack, "Test red-black tree functionality")

#define RBT_ELEM_CNT	32

/* this is so complicated because RBT_ELEM_CNT is likely the maximum number
 * of bits representable in an unsigned long, so we have to look out for
 * overflows.
 */
#define RBT_ELEM_MASK	((((0x01lu << (RBT_ELEM_CNT - 1)) - 1) << 1) | 0x01lu)

/* recursive routine for obtaining pre-order list... */
static void
_make_pre(rb_node_t *node, int *order, int *idx, int reverse,
	  unsigned long *visited)
{
  order[(*idx)++] = (int)rn_value(node); /* store node... */
  if (reverse && rn_right(node)) /* recurse down the right if reversed */
    _make_pre(rn_right(node), order, idx, reverse, visited);
  if (rn_left(node)) /* recurse down the left */
    _make_pre(rn_left(node), order, idx, reverse, visited);
  if (!reverse && rn_right(node)) /* recurse down the right if not reversed */
    _make_pre(rn_right(node), order, idx, reverse, visited);

  *visited &= ~(0x01ul << (int)rn_value(node));
}

/* recursive routine for obtaining in-order list... */
static void
_make_in(rb_node_t *node, int *order, int *idx, int reverse,
	 unsigned long *visited)
{
#define selnode(n, rev)	((rev) ? rn_right(n) : rn_left(n))
  if (selnode(node, reverse)) /* recurse down one subtree... */
    _make_in(selnode(node, reverse), order, idx, reverse, visited);
  order[(*idx)++] = (int)rn_value(node); /* store node... */
  if (selnode(node, !reverse)) /* recurse down other subtree... */
    _make_in(selnode(node, !reverse), order, idx, reverse, visited);

  *visited &= ~(0x01ul << (int)rn_value(node));
}

/* recursive routine for obtaining post-order list... */
static void
_make_post(rb_node_t *node, int *order, int *idx, int reverse,
	   unsigned long *visited)
{
  if (reverse && rn_right(node)) /* recurse down the right if reversed */
    _make_post(rn_right(node), order, idx, reverse, visited);
  if (rn_left(node)) /* recurse down the left */
    _make_post(rn_left(node), order, idx, reverse, visited);
  if (!reverse && rn_right(node)) /* recurse down the right if not reversed */
    _make_post(rn_right(node), order, idx, reverse, visited);
  order[(*idx)++] = (int)rn_value(node); /* store node... */

  *visited &= ~(0x01ul << (int)rn_value(node));
}

/* calculate the order of nodes in the traversal of an redblack tree */
static void
make_order(rb_tree_t *tree, int *order, unsigned long flags,
	   unsigned long *visited)
{
  int idx = 0;

  switch (flags & RBT_ORDER_MASK) {
  case RBT_ORDER_PRE: /* get the pre-order list */
    _make_pre(rt_root(tree), order, &idx, flags & DB_FLAG_REVERSE, visited);
    break;
  case RBT_ORDER_IN: /* get the in-order list */
    _make_in(rt_root(tree), order, &idx, flags & DB_FLAG_REVERSE, visited);
    break;
  case RBT_ORDER_POST: /* get the post-order list */
    _make_post(rt_root(tree), order, &idx, flags & DB_FLAG_REVERSE, visited);
    break;
  }
}

/* compare two sets of orders... */
static int
comp_order(int *o1, int *o2)
{
  int i;

  for (i = 0; i < RBT_ELEM_CNT; i++) /* walk through all the elements */
    if (o1[i] != o2[i])
      return 1; /* oops, mismatch... */

  return 0; /* OK, everything's fine. */
}

/* handy macro to generate the arguments for identifying nodes in printf's */
#define n_debargs(node) (void *)(node), (node) ? dk_len(rn_key(node)) : -1, \
			rn_isblack(node) ? "BLACK" : \
			(rn_isred(node) ? "RED" : "NO COLOR")

/* check that a tree really is a red-black tree */
static int
treecheck(rb_node_t *node, rb_node_t *parent, int bh, int *tbhp)
{
  int init_bh = -1; /* tree black height */

  if (!node) /* an empty tree is a red-black tree... */
    return 1;

  /* First, check parental referential integrity... */
  if (node->rn_parent != parent) {
    fprintf(stderr, "Node %p/%d (%s) thinks its a child of %p/%d (%s) "
	    "rather than of %p/%d (%s)\n", n_debargs(node),
	    n_debargs(node->rn_parent), n_debargs(parent));
    return 0;
  }

  /* Next, make sure it has a reasonable color... */
  if (!rn_isred(node) && !rn_isblack(node)) {
    fprintf(stderr, "Node %p/%d (%s) is neither red nor black\n",
	    n_debargs(node));
    return 0;
  }

  /* Now, if it's at the root of the tree, it'd better be black */
  if (!parent && !rn_isblack(node)) {
    fprintf(stderr, "Node %p/%d (%s) is a non-black root node\n",
	    n_debargs(node->rn_parent));
    return 0;
  }

  /* No red node may have a red parent */
  if (rn_isred(parent) && rn_isred(node)) {
    fprintf(stderr, "Node %p/%d (%s) is a red node with a red parent "
	    "%p/%d (%s)\n", n_debargs(node), n_debargs(parent));
    return 0;
  }

  if (!tbhp) /* need a pointer to the total black height... */
    tbhp = &init_bh; /* so use ours */

  if (parent && rn_isblack(node)) /* increment black height (for children) */
    bh++;

  /* Check that the left node is part of a red-black tree */
  if (node->rn_left) {
    /* First, double-check node ordering... */
    if (dk_len(rn_key(node->rn_left)) >= dk_len(rn_key(node))) {
      fprintf(stderr, "Node %p/%d (%s) is less than its left child, "
	      "%p/%d (%s)", n_debargs(node), n_debargs(node->rn_left));
      return 0;
    } else if (!treecheck(node->rn_left, node, bh, tbhp))
     return 0;
  } else if (*tbhp < 0) /* hit an external node for the first time... */
    *tbhp = bh + 1; /* store the black height */
  else if ((bh + 1) != *tbhp) { /* verify that black heights are consistent */
    fprintf(stderr, "Node %p/%d (%s) left child is leaf with bh %d "
	    "(expected %d)\n", n_debargs(node), bh + 1, *tbhp);
    return 0;
  }

  /* Check that the right node is part of a red-black tree */
  if (node->rn_right) {
    /* First, double-check node ordering... */
    if (dk_len(rn_key(node)) >= dk_len(rn_key(node->rn_right))) {
      fprintf(stderr, "Node %p/%d (%s) is greater than its right child, "
	      "%p/%d (%s)", n_debargs(node), n_debargs(node->rn_left));
      return 0;
    } else if (!treecheck(node->rn_right, node, bh, tbhp))
      return 0;
  } else if (*tbhp < 0) /* hit an external node for the first time... */
    *tbhp = bh + 1; /* store the black height */
  else if ((bh + 1) != *tbhp) { /* verify that black heights are consistent */
    fprintf(stderr, "Node %p/%d (%s) right child is leaf with bh %d "
	    "(expected %d)\n", n_debargs(node), bh + 1, *tbhp);
    return 0;
  }

  /* at this point, if we're the root, the black height had better be set */
  assert(parent || *tbhp > 0);

  /* Informational message... */
  if (!parent)
    fprintf(stderr, "Tree black height: %d\n", *tbhp);

  return 1; /* it's a red-black tree. */
}

/* simple comparison function for testing... */
static long
t_comp(rb_tree_t *tree, db_key_t *key1, db_key_t *key2)
{
  return dk_len(key1) - dk_len(key2); /* difference of the lengths */
}

int
main(int argc, char **argv)
{
  unsigned long err;
  rb_tree_t tree; /* a red-black tree to play with... */
  rb_node_t nodes[RBT_ELEM_CNT];
  rb_node_t *n = 0;
  db_key_t key = DB_KEY_INIT(0, 0);
  int i, tmp;
  unsigned long visited, visit_init;

  srand(time(0)); /* seed random number generator */

  /* Let's test rt_init() first... */
  TEST(t_redblack, rt_init, "Test that rt_init() may be called",
       (!(err = rt_init(&tree, t_comp, 0))), FATAL(0),
       ("rt_init() call successful"),
       ("rt_init() call failed with error %lu", err));

  /* Now try some rn_init()s... */
  TEST_DECL(t_redblack, rn_init, "Test that rn_init() may be called")
  for (i = 0; i < RBT_ELEM_CNT; i++)
    if ((err = rn_init(&nodes[i], (void *)i)))
      FAIL(TEST_NAME(rn_init), FATAL(0), "rn_init() failed with error %lu",
	   err);
  PASS(TEST_NAME(rn_init), "rn_init() calls successful");

  /* OK, now we gotta add some nodes to the tree with rt_add()... */
  TEST_DECL(t_redblack, rt_add, "Test that rt_add() adds entries to the tree")
  for (dk_len(&key) = 0; dk_len(&key) < RBT_ELEM_CNT; dk_len(&key)++)
    if ((err = rt_add(&tree, &nodes[dk_len(&key)], &key)))
      FAIL(TEST_NAME(rt_add), FATAL(0), "rt_add() failed with error %lu", err);
    else if (!treecheck(rt_root(&tree), 0, 0, 0))
      FAIL(TEST_NAME(rt_add), FATAL(0), "rt_add() corrupted tree's "
	   "red-black property");
  PASS(TEST_NAME(rt_add), "rt_add() calls successful");

  /* Next, try a few rt_find()s... */
  TEST_DECL(t_redblack, rt_find, "Test that rt_find() finds entries in tree")
  for (visited = RBT_ELEM_MASK; visited;
       visited &= ~(0x01lu << dk_len(&key))) {
    dk_len(&key) = rand() % RBT_ELEM_CNT; /* select an entry at random... */
    fprintf(stderr, "Looking up entry %d . . .\n", dk_len(&key));
    if ((err = rt_find(&tree, &n, &key)))
      FAIL(TEST_NAME(rt_find), FATAL(0), "rt_find() failed with error %lu",
	   err);
    else if (n != &nodes[dk_len(&key)])
      FAIL(TEST_NAME(rt_find), FATAL(0), "rt_find() found wrong entry; "
	   "expected %d returned %d", dk_len(&key), n ? -1 : n - nodes);
  }
  PASS(TEST_NAME(rt_find), "rt_find() calls successful");

  /* Let's try removing some nodes with rt_remove()... */
  TEST_DECL(t_redblack, rt_remove, "Test that rt_remove() removes nodes")
  for (visit_init = RBT_ELEM_MASK; rt_count(&tree) > RBT_ELEM_CNT / 2;
       visit_init &= ~(0x01lu << tmp)) {
    tmp = rand() % RBT_ELEM_CNT; /* select an entry at random... */

    if (!(visit_init & (0x01lu << tmp))) /* already removed... */
      continue; /* so skip it */

    fprintf(stderr, "Removing element %d . . .\n", tmp);

    if ((err = rt_remove(&tree, &nodes[tmp])))
      FAIL(TEST_NAME(rt_remove), FATAL(0), "rt_remove() failed with error %lu",
	   err);
    else if (!treecheck(rt_root(&tree), 0, 0, 0))
      FAIL(TEST_NAME(rt_remove), FATAL(0), "rt_remove() corrupted tree's "
	   "red-black property");
  }
  /* Now see if we can still locate elements in the tree... */
  for (visited = RBT_ELEM_MASK; visited;
       visited &= ~(0x01lu << dk_len(&key))) {
    dk_len(&key) = rand() % RBT_ELEM_CNT; /* select an entry at random... */
    fprintf(stderr, "Looking up entry %d . . .%s\n", dk_len(&key),
	    (visit_init & (0x01lu << dk_len(&key))) ? "" : "[removed]");

    err = rt_find(&tree, &n, &key); /* look up the node */

    if (err != ((visit_init & (0x01lu << dk_len(&key))) ? 0 : DB_ERR_NOENTRY))
      FAIL(TEST_NAME(rt_remove), FATAL(0), "rt_remove() (probably) corrupted "
	   "the tree bad enough that rt_find() returned error %lu", err);
  }
  PASS(TEST_NAME(rt_remove), "rt_remove() calls successful");

  return 0;
}
