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
 * \brief Flush a red-black tree.
 *
 * This function flushes a red-black tree--that is, it removes each
 * node from the tree.  If a \p flush_func is specified, it will be
 * called on the node after it has been removed from the tree, and may
 * safely call <CODE>free()</CODE>.
 *
 * \param tree	A pointer to a #rb_tree_t.
 * \param flush_func
 *		A pointer to a callback function used to perform
 *		user-specified actions on a node after removing it
 *		from the tree.  May be \c NULL.  See the documentation
 *		for #rb_iter_t for more information.
 * \param extra	A \c void pointer that will be passed to \p
 *		flush_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The red-black tree is frozen.
 */
unsigned long
rt_flush(rb_tree_t *tree, rb_iter_t flush_func, void *extra)
{
  unsigned long retval = 0;
  rb_node_t *node;

  initialize_dbpr_error_table(); /* initialize error table */

  if (!rt_verify(tree)) /* verify arguments */
    return DB_ERR_BADARGS;

  if (tree->rt_flags & RBT_FLAG_FREEZE) /* don't mess with frozen trees */
    return DB_ERR_FROZEN;

  /* OK, walk through the nodes... */
  while (tree->rt_root) {
    rt_remove(tree, node = tree->rt_root); /* remove the node... */

    if (flush_func) {
      tree->rt_flags |= RBT_FLAG_FREEZE; /* freeze the table... */

      retval = (*flush_func)(tree, node, extra); /* call flush function */

      tree->rt_flags &= ~RBT_FLAG_FREEZE; /* unfreeze table... */

      if (retval) /* if flush function failed, error out */
	return retval;
    }
  }

  tree->rt_count = 0; /* clear the node count */
  tree->rt_root = 0; /* and the root node... */

  return 0;
}
