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
 * \brief Implementation of st_free().
 *
 * This file contains the implementation of the st_free() function,
 * used to release memory allocated by the sparse matrix hash table.
 */
#include "dbprim.h"
#include "dbprim_int.h"

db_err_t
st_free(smat_table_t *table)
{
  if (!st_verify(table)) /* verify argument */
    return DB_ERR_BADARGS;

  return ht_free(&table->st_table); /* call out to hash */
}
