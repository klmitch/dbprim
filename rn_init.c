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
 * \brief Dynamically initialize a red-black tree node.
 *
 * This function dynamically initializes a red-black tree node.
 *
 * \param node	A pointer to a #rb_tree_t to be initialized.
 * \param value	A pointer to \c void which will be the value of the
 *		red-black tree entry.
 *
 * \retval DB_ERR_BADARGS	A \c NULL pointer was passed for \p
 *				node.
 */
unsigned long
rn_init(rb_node_t *node, void *value)
{
  initialize_dbpr_error_table(); /* initialize error table */

  if (!node) /* verify arguments */
    return DB_ERR_BADARGS;

  /* initialize the node */
  node->rn_color = RB_COLOR_NONE;
  node->rn_tree = 0;
  node->rn_parent = 0;
  node->rn_left = 0;
  node->rn_right = 0;
  node->rn_key.dk_key = 0;
  node->rn_key.dk_len = 0;
  node->rn_value = value;

  node->rn_magic = RB_NODE_MAGIC; /* set the magic number */

  return 0;
}
