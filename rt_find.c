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
 * \brief Find an entry in a red-black table.
 *
 * This function looks up an entry matching the given \p key.
 *
 * \param tree	A pointer to a #rb_tree_t.
 * \param node_p
 *		A pointer to a pointer to a #rb_node_t.  This is a
 *		result parameter.  If \c NULL is passed, the lookup
 *		will be performed and an appropriate error code
 *		returned.
 * \param key	A pointer to a #db_key_t describing the item to find.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_NOENTRY	No matching entry was found.
 */
unsigned long
rt_find(rb_tree_t *tree, rb_node_t **node_p, db_key_t *key)
{
  rb_node_t *node;

  initialize_dbpr_error_table(); /* initialize error table */

  if (!rt_verify(tree) || !key) /* verify arguments */
    return DB_ERR_BADARGS;

  if (!tree->rt_count) /* no entries in table... */
    return DB_ERR_NOENTRY;

  if (!(node = _rb_locate(tree, 0, key))) /* look up the node... */
    return DB_ERR_NOENTRY; /* err, it's not in the tree... */

  if (node_p) /* found one, return it */
    *node_p = node;

  return 0;
}
