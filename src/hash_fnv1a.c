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
 * \brief Implementation of hash_fnv1a().
 *
 * This file contains the implementation of the hash_fnv1a() function,
 * a generic hash function callback implementing the FNV-1a hash
 * algorithm.
 */
#include "hashtab_int.h"

hash_t
hash_fnv1a(hash_table_t *table, db_key_t *key)
{
  hash_fnv_state_t state;

  if (!key || !dk_len(key) || !dk_key(key)) /* invalid key?  return 0 */
    return 0;

  hash_fnv_init(&state);
  hash_fnv1a_accum(&state, dk_key(key), dk_len(key));
  return hash_fnv_final(&state);
}
