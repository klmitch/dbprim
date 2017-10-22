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
/** \internal
 * \file
 * \brief Implementation of dbprim_err().
 *
 * This file contains the implementation of the dbprim_err() function,
 * which is used to convert error return codes into error strings.
 */

#include "common_int.h"

static const char * const text[] = {
  "Bad arguments",
  "Element already in use",
  "Element is not in use",
  "Element is not in this table",
  "No matching element",
  "Duplicate keys found",
  "Table not empty",
  "Table is frozen; try again later",
  "No table allocated",
  "Failed to re-add entry to table",
  "Unrecoverable error; table unusable"
};

const char *
dbprim_err(db_err_t err)
{
  if (err < DB_ERR_BADARGS || err > DB_ERR_UNRECOVERABLE)
    return 0;

  /* Normalize the error code and return the appropriate text */
  return text[err - DB_ERR_BADARGS];
}
