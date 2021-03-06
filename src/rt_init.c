/*
** Copyright (C) 2003, 2017 by Kevin L. Mitchell <klmitch@mit.edu>
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
/** \internal
 * \file
 * \brief Implementation of rt_init().
 *
 * This file contains the implementation of the rt_init() function,
 * used to dynamically initialize a red-black tree.
 */
#include "redblack_int.h"

db_err_t
rt_init(rb_tree_t *tree, rb_comp_t comp, void *extra)
{
  if (!tree || !comp) /* verify arguments */
    return DB_ERR_BADARGS;

  /* initialize the tree */
  tree->rt_flags = 0;
  tree->rt_count = 0;
  tree->rt_root = 0;
  tree->rt_comp = comp;
  tree->rt_extra = extra;

  tree->rt_magic = RB_TREE_MAGIC; /* set the magic number */

  return 0;
}
