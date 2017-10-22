/*
** Copyright (C) 2002, 2017 by Kevin L. Mitchell <klmitch@mit.edu>
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
 * \brief Implementation of ll_flush().
 *
 * This file contains the implementation of the ll_flush() function,
 * used to remove every element from a linked list.
 */
#include "dbprim_int.h"

db_err_t
ll_flush(link_head_t *list, link_iter_t flush_func, void *extra)
{
  link_elem_t *elem;
  db_err_t retval;

  if (!ll_verify(list)) /* Verify arguments */
    return DB_ERR_BADARGS;

  while ((elem = list->lh_first)) { /* Walk through the list... */
    ll_remove(list, elem); /* remove the element */
    /* call flush function, erroring out if it fails */
    if (flush_func && (retval = (*flush_func)(list, elem, extra)))
      return retval;
  }

  list->lh_count = 0; /* clear the list head */
  list->lh_first = 0;
  list->lh_last = 0;

  return 0;
}
