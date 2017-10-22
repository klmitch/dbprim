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
#ifndef included_dbprim_hashtab_int_h__
#define included_dbprim_hashtab_int_h__

/** \internal
 * \file
 * \brief Hash tables internal header file.
 *
 * This header file contains the internal definitions for the hash tables
 * component of the Database Primitives library.
 */

#include "common_int.h"
#include "hashtab.h"

/** \internal
 * \ingroup dbprim_hash
 * \brief Select a prime number.
 *
 * This function is used by the hash table system to find the first
 * prime number larger than \p start.  This prime number will be used
 * as the hash table modulus.
 *
 * \param[in]		start	The number from which to start looking
 *				for the next largest prime.
 *
 * \return	The first prime number larger than \p start.
 */
hash_t _hash_prime(hash_t start);

/** \internal
 * \ingroup dbprim_hash
 * \brief Select hash table roll over size.
 *
 * This macro is used to compute the "roll over" size--the size at
 * which the hash table will be grown (assuming that the table has
 * #HASH_FLAG_AUTOGROW set).
 *
 * \param[in]		mod	The table modulus.
 *
 * \return	The "roll over" size.
 */
#define _hash_rollover(mod)	(((mod) * 4) / 3)

/** \internal
 * \ingroup dbprim_hash
 * \brief Select hash table roll under size.
 *
 * This macro is used to compute the "roll under" size--the size at
 * which the hash table will be shrunk (assuming that the table has
 * #HASH_FLAG_AUTOSHRINK set).
 *
 * \param[in]		mod	The table modulus.
 *
 * \return	The "roll under" size.
 */
#define _hash_rollunder(mod)	(((mod) * 3) / 4)

/** \internal
 * \ingroup dbprim_hash
 * \brief Fuzz the initial hash table size.
 *
 * This macro is used to apply a fudge factor to a user-supplied size
 * for the hash table, causing a slightly larger bucket to be
 * allocated.
 *
 * \param[in]		mod	The requested table modulus.
 *
 * \return	The "fuzzed" size.
 */
#define _hash_fuzz(mod)		(((mod) * 4) / 3)

/** \internal
 * \ingroup dbprim_hash
 * \brief FNV offset basis parameter.
 *
 * This is the 32-bit offset basis for the FNV hash algorithm.  See
 * http://www.isthe.com/chongo/tech/comp/fnv/ for more information
 * about the FNV hash algorithm.
 */
#define HASH_FNV_OFFSET		2166136261UL

/** \internal
 * \ingroup dbprim_hash
 * \brief FNV prime parameter.
 *
 * This is the 32-bit multiplication prime for the FNV hash algorithm.
 * See http://www.isthe.com/chongo/tech/comp/fnv/ for more information
 * about the FNV hash algorithm.
 */
#define HASH_FNV_PRIME		16777619UL

#endif /* included_dbprim_hashtab_int_h__ */
