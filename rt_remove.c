/*
** Copyright (C) 2003 by Kevin L. Mitchell <klmitch@mit.edu>
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
#include "dbprim.h"
#include "dbprim_int.h"

RCSTAG("@(#)$Id$");

/** \ingroup dbprim_rbtree
 * \brief Remove a node from a red-black tree.
 *
 * This function removes the given node from the specified red-black
 * tree.
 *
 * \param tree	A pointer to a #rb_tree_t.
 * \param node	A pointer to a #rb_node_t to be removed from the tree.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_UNUSED	Node is not in a red-black tree.
 * \retval DB_ERR_WRONGTABLE	Node is not in this tree.
 * \retval DB_ERR_FROZEN	Red-black tree is frozen.
 */
unsigned long
rt_remove(rb_tree_t *tree, rb_node_t *node)
{
  rb_color_t col;
  rb_node_t *t1, *t2, *t2p = 0;

  initialize_dbpr_error_table(); /* initialize error table */

  if (!rt_verify(tree) || !rn_verify(node)) /* verify arguments */
    return DB_ERR_BADARGS;

  if (!node->rn_tree) /* it's not in a tree... */
    return DB_ERR_UNUSED;
  if (node->rn_tree != tree) /* it's in the wrong tree */
    return DB_ERR_WRONGTABLE;

  if (tree->rt_flags & RBT_FLAG_FREEZE) /* don't remove from frozen trees */
    return DB_ERR_FROZEN;

  /* OK, this logic is tricky--first, let's remove the node */
  if (!node->rn_left || !node->rn_right)
    t1 = node; /* node to be deleted has only one child */
  else
    for (t1 = node->rn_right; t1->rn_left; t1 = t1->rn_left)
      ; /* Find the left-most node of the right child of deleted node */

  t2 = t1->rn_left ? t1->rn_left : t1->rn_right; /* child of node... */

  if (t2)
    t2->rn_parent = t1->rn_parent; /* update the parent pointer */
  else
    t2p = t1->rn_parent; /* must remember the parent for rebalancing */

  if (t1->rn_parent) { /* update the parent of node... */
    if (rn_isleft(t1))
      t1->rn_parent->rn_left = t2;
    else
      t1->rn_parent->rn_right = t2;
  } else /* it's at the root of the tree... */
    tree->rt_root = t2;

  col = t1->rn_color; /* cache the color... */

  if (t1 != node) { /* t1 replaces the node to be deleted */
    t1->rn_color = node->rn_color;
    t1->rn_parent = node->rn_parent;
    t1->rn_left = node->rn_left;
    t1->rn_right = node->rn_right;
    if (node->rn_parent) { /* OK, update the parent's pointer... */
      if (rn_isleft(node))
	t1->rn_parent->rn_left = t1;
      else
	t1->rn_parent->rn_right = t1;
    } else /* it was at the root of the tree... */
      tree->rt_root = t1;
  }

  /* OK, now we rebalance the tree if necessary.  t2parent is a special
   * macro to select either t2->rn_parent or t2p, if t2 is 0.
   */
#define t2parent	(t2 ? t2->rn_parent : t2p)
  if (col == RB_COLOR_BLACK) {
    while (t2parent && rn_isblack(t2)) /* while balancing neccessary... */
      if (t2 && rn_isleft(t2)) { /* node is a left child... */
	t1 = t2parent->rn_right;
	if (rn_isred(t1)) {
	  t1->rn_color = RB_COLOR_BLACK; /* recolor the nodes... */
	  t2parent->rn_color = RB_COLOR_RED;
	  _rb_rotate(tree, t2parent->rn_right); /* move the right child up */
	  t1 = t2parent->rn_right;
	}

	/* If both children are black... */
	if (rn_isblack(t1->rn_left) && rn_isblack(t1->rn_right)) {
	  t1->rn_color = RB_COLOR_RED; /* recolor the node red */
	  t2 = t2parent; /* move up the tree a bit */
	} else {
	  if (rn_isblack(t1->rn_right)) {
	    t1->rn_left->rn_color = RB_COLOR_BLACK; /* recolor nodes... */
	    t1->rn_color = RB_COLOR_RED;
	    _rb_rotate(tree, t1->rn_left); /* move left child up a level */
	    t1 = t2parent->rn_right; /* and move to other child of t2 */
	  }

	  t1->rn_color = t2parent->rn_color; /* recolor nodes... */
	  t2parent->rn_color = RB_COLOR_BLACK;
	  t1->rn_right->rn_color = RB_COLOR_BLACK;
	  _rb_rotate(tree, t2parent->rn_right); /* move child up a level */
	  t2 = tree->rt_root;
	}
      } else { /* node is a right child... */
	t1 = t2parent->rn_left;
	if (rn_isred(t1)) {
	  t1->rn_color = RB_COLOR_BLACK; /* recolor the nodes... */
	  t2parent->rn_color = RB_COLOR_RED;
	  _rb_rotate(tree, t2parent->rn_left); /* move the left child up */
	  t1 = t2parent->rn_left;
	}

	/* If both children are black... */
	if (rn_isblack(t1->rn_right) && rn_isblack(t1->rn_left)) {
	  t1->rn_color = RB_COLOR_RED; /* recolor the node red */
	  t2 = t2parent; /* move up the tree a bit */
	} else {
	  if (rn_isblack(t1->rn_left)) {
	    t1->rn_right->rn_color = RB_COLOR_BLACK; /* recolor nodes... */
	    t1->rn_color = RB_COLOR_RED;
	    _rb_rotate(tree, t1->rn_right); /* move right child up a level */
	    t1 = t2parent->rn_left; /* and move to other child of t2 */
	  }

	  t1->rn_color = t2parent->rn_color; /* recolor nodes... */
	  t2parent->rn_color = RB_COLOR_BLACK;
	  t1->rn_left->rn_color = RB_COLOR_BLACK;
	  _rb_rotate(tree, t2parent->rn_left); /* move child up a level */
	  t2 = tree->rt_root;
	}
      }

    if (t2) /* recolor node... */
      t2->rn_color = RB_COLOR_BLACK;
  }

  node->rn_color = RB_COLOR_NONE; /* clear the node */
  node->rn_tree = 0;
  node->rn_parent = 0;
  node->rn_left = 0;
  node->rn_right = 0;

  tree->rt_count--; /* update the node count */

  return 0;
}
