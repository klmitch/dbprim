/*
** Copyright (C) 2017 by Kevin L. Mitchell <klmitch@mit.edu>
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
*/
#ifndef included_dbprim_redblack_int_h__
#define included_dbprim_redblack_int_h__

/** \internal
 * \file
 * \brief Red-black trees internal header file.
 *
 * This header file contains the internal definitions for the
 * red-black tree component of the Database Primitives library.
 */

#include "common_int.h"
#include "redblack.h"

/** \internal
 * \ingroup dbprim_rbtree
 * \brief Locate or insert a red-black tree node.
 *
 * This function is used to locate a red-black tree node with a key
 * matching \p key.  If the node does not exist, but \p node is
 * non-<CODE>NULL</CODE>, \p node will be inserted into the tree at an
 * appropriate place, although please note that rebalancing will be
 * necessary.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		node	A pointer to a #rb_node_t to be added
 *				to the tree.
 * \param[in]		key	A pointer to a #db_key_t containing
 *				the key to be looked up or inserted.
 *
 * \return	A pointer to the #rb_node_t found or inserted, or \c
 *		NULL if one could not be found.
 */
rb_node_t *_rb_locate(rb_tree_t *tree, rb_node_t *node, db_key_t *key);

/** \internal
 * \ingroup dbprim_rbtree
 * \brief Rotate tree nodes.
 *
 * This function is used to swap \p child with its parent, effecting a
 * tree-balancing rotation.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		child	A pointer to a #rb_node_t to be
 *				swapped with its parent.
 */
void _rb_rotate(rb_tree_t *tree, rb_node_t *child);

#endif /* included_dbprim_redblack_int_h__ */
