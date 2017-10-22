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
 * \brief Implementation of hash_fnv1().
 *
 * This file contains the implementation of the hash_fnv1() function,
 * a generic hash function callback implementing the FNV-1 hash
 * algorithm.
 */
#include "dbprim.h"
#include "dbprim_int.h"

hash_t
hash_fnv1(hash_table_t *table, db_key_t *key)
{
  int i;
  hash_t hash = HASH_FNV_OFFSET;
  unsigned char *c;

  if (!key || !dk_len(key) || !dk_key(key)) /* invalid key?  return 0 */
    return 0;

  c = (unsigned char *)dk_key(key);
  for (i = 0; i < dk_len(key); i++, c++) { /* FNV-1 algorithm... */
    hash *= HASH_FNV_PRIME; /* multiply by the prime... */
    hash &= 0xffffffff; /* reduce to 32-bit */
    hash ^= *c; /* hash in the data octet */
  }

  return hash;
}
