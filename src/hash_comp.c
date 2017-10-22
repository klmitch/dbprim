/*
** Copyright (C) 2006, 2017 by Kevin L. Mitchell <klmitch@mit.edu>
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
 * \brief Implementation of hash_comp().
 *
 * This file contains the implementation of the hash_comp() function,
 * a generic hash comparison callback utilizing memcmp().
 */
#include <string.h>

#include "dbprim_int.h"

int
hash_comp(hash_table_t *table, db_key_t *key1, db_key_t *key2)
{
  if (!key1 || !dk_len(key1) || !dk_key(key1) || /* invalid keys? */
      !key2 || !dk_len(key2) || !dk_key(key2))
    return 1; /* return "no match" */

  /* Compare lengths and keys... */
  return dk_len(key1) != dk_len(key2) ||
    memcmp(dk_key(key1), dk_key(key2), dk_len(key1));
}
