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
 * \brief Move a node in a red-black tree.
 *
 * This function moves an existing node in the red-black tree to
 * correspond to the new key.
 *
 * \param tree	A pointer to a #rb_tree_t.
 * \param node	A pointer to a #rb_node_t to be moved.  It must
 *		already be in the red-black tree.
 * \param key	A pointer to a #db_key_t describing the new key for
 *		the node.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_UNUSED	Node is not in a red-black tree.
 * \retval DB_ERR_WRONGTABLE	Node is not in this tree.
 * \retval DB_ERR_FROZEN	Red-black tree is frozen.
 * \retval DB_ERR_DUPLICATE	New key is a duplicate of an existing
 *				key.
 * \retval DB_ERR_READDFAILED	Unable to re-add node to tree.
 */
unsigned long
rt_move(rb_tree_t *tree, rb_node_t *node, db_key_t *key)
{
  unsigned long retval;

  initialize_dbpr_error_table(); /* initialize error table */

  if (!rt_verify(tree) || !rn_verify(node) || !key) /* verify arguments */
    return DB_ERR_BADARGS;

  if (!node->rn_tree) /* it's not in a tree */
    return DB_ERR_UNUSED;
  if (node->rn_tree != tree) /* it's in the wrong tree */
    return DB_ERR_WRONGTABLE;

  if (tree->rt_flags & RBT_FLAG_FREEZE) /* don't mess with frozen trees */
    return DB_ERR_FROZEN;

  if (_rb_locate(tree, 0, key)) /* don't permit duplicates */
    return DB_ERR_DUPLICATE;

  /* OK, remove node from the tree... */
  if ((retval = rt_remove(tree, node)))
    return retval;

  /* Now re-add it to the tree... */
  return rt_add(tree, node, key) ? DB_ERR_READDFAILED : 0;
}
