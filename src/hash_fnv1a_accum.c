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
 * \brief Implementation of hash_fnv1a_accum().
 *
 * This file contains the implementation of the hash_fnv1a_accum()
 * function, which updates a #hash_fnv_state_t with the passed-in
 * data.
 */
#include "hashtab_int.h"

db_err_t
hash_fnv1a_accum(hash_fnv_state_t *state, void *data, size_t len)
{
  unsigned char *c;

  if (!state || !data || !len) /* Sanity-check the arguments */
    return DB_ERR_BADARGS;

  /* Accumulate the data */
  for (c = (unsigned char *)data; len; c++, len--) {
    *state ^= *c; /* hash the data octet */
    *state *= HASH_FNV_PRIME; /* multiply by the prime */
  }

  return 0;
}
