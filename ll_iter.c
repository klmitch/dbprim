/*
** Copyright (C) 2002 by Kevin L. Mitchell <klmitch@mit.edu>
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

/** \ingroup dbprim_link
 * \brief Iterate over each entry in a linked list.
 *
 * This function iterates over a linked list, executing the given \p
 * iter_func for each entry.
 *
 * \param list	A pointer to a #link_head_t.
 * \param start	A pointer to a #link_elem_t describing where in the
 *		linked list to start.  If \c NULL is passed, the
 *		beginning of the list will be assumed.
 * \param iter_func
 *		A pointer to a callback function used to perform
 *		user-specified actions on an element in a linked
 *		list.  \c NULL is an invalid value.  See the
 *		documentation for #link_iter_t for more information.
 * \param extra	A \c void pointer that will be passed to \p
 *		iter_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this linked list.
 */
unsigned long
ll_iter(link_head_t *list, link_elem_t *start,
	link_iter_t iter_func, void *extra)
{
  unsigned long retval;
  link_elem_t *elem, *next = 0;

  initialize_dbpr_error_table(); /* initialize error table */

  /* verify arguments */
  if (!ll_verify(list) || (start && !le_verify(start)) || !iter_func)
    return DB_ERR_BADARGS;

  /* If the start element is specified, check that it's in this list... */
  if (start && list != start->le_head)
    return DB_ERR_WRONGTABLE;

  /* Walk through list and return first non-zero return value */
  for (elem = start ? start : list->lh_first; elem; elem = next) {
    next = elem->le_next; /* iter_func may ll_remove... */
    if ((retval = (*iter_func)(list, elem, extra)))
      return retval;
  }

  return 0;
}
