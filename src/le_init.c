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
 * \brief Implementation of le_init().
 *
 * This file contains the implementation of the le_init() function,
 * used to dynamically initialize a linked list element.
 */
#include "linklist_int.h"

db_err_t
le_init(link_elem_t *elem, void *object)
{
  if (!elem) /* verify arguments */
    return DB_ERR_BADARGS;

  elem->le_next = 0; /* initialize the element */
  elem->le_prev = 0;
  elem->le_object = object;
  elem->le_head = 0;
  elem->le_flags = 0;

  elem->le_magic = LINK_ELEM_MAGIC; /* set the magic number */

  return 0;
}
