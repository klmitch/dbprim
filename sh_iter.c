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

struct _sh_iter_s {
  smat_table_t *si_table;	/* pointer to the smat table */
  smat_iter_t	si_iter;	/* iter function */
  void	       *si_extra;	/* extra data */
};

static unsigned long
_sh_iter_iter(link_head_t *head, link_elem_t *elem, void *extra)
{
  struct _sh_iter_s *si;

  si = extra;

  /* call the user iteration function--with appropriate translation */
  return (*si->si_iter)(si->si_table, le_object(elem), si->si_extra);
}

/** \ingroup dbprim_smat
 * \brief Iterate over each entry in a row or column of a sparse
 * matrix.
 *
 * This function iterates over a row or column of a sparse matrix,
 * executing the given \p iter_func for each entry.
 *
 * \param head	A pointer to a #smat_head_t.
 * \param start	A pointer to a #smat_entry_t describing where in the
 *		row or column to start.  If \c NULL is passed, the
 *		beginning of the row or column will be assumed.
 * \param iter_func
 *		A pointer to a callback function used to perform
 *		user-specified actions on an entry in a row or column
 *		of a sparse matrix.  \c NULL is an invalid value.  See
 *		the documentation for #smat_iter_t for more
 *		information.
 * \param extra	A \c void pointer that will be passed to \p
 *		iter_func.
 * \param flags	If #DB_FLAG_REVERSE is given, iteration will be done
 *		from the end of the list backwards towards the head.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this row or column.
 */
unsigned long
sh_iter(smat_head_t *head, smat_entry_t *start,
	smat_iter_t iter_func, void *extra, unsigned long flags)
{
  struct _sh_iter_s si;

  initialize_dbpr_error_table(); /* initialize error table */

  /* verify arguments */
  if (!sh_verify(head) || (start && !se_verify(start)) || !iter_func)
    return DB_ERR_BADARGS;

  /* initialize extra data... */
  si.si_table = head->sh_table;
  si.si_iter = iter_func;
  si.si_extra = extra;

  /* call into linked list library to iterate over the list */
  return ll_iter(&head->sh_head, start ? &start->se_link[head->sh_elem] : 0,
		 _sh_iter_iter, &si, flags);
}
