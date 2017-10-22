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
 * \brief Implementation of hash_fnv_init().
 *
 * This file contains the implementation of the hash_fnv_init()
 * function, which initializes a #hash_fnv_state_t prior to calls to
 * hash_fnv1_accum() or hash_fnv1a_accum().
 */
#include "hashtab_int.h"

db_err_t
hash_fnv_init(hash_fnv_state_t *state)
{
  if (!state) /* Sanity-check the arguments */
    return DB_ERR_BADARGS;

  /* Initialize the state */
  *state = HASH_FNV_OFFSET;

  return 0;
}
