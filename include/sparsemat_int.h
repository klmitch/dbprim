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
#ifndef included_dbprim_sparsemat_int_h__
#define included_dbprim_sparsemat_int_h__

/** \internal
 * \file
 * \brief Sparse matrices internal header file.
 *
 * This header file contains the internal definitions for the sparse
 * matrices component of the Database Primitives library.
 */

#include "common_int.h"
#include "sparsemat.h"

/** \internal
 * \ingroup dbprim_smat
 * \brief Remove an entry from a sparse matrix (internal).
 *
 * This function implements the actual logic that removes a sparse
 * matrix entry from a sparse matrix table.  Sparse matrix entries
 * must be removed from three different locations before they can be
 * passed to free(), and there are several places in the library where
 * they are removed from one location and must subsequently be removed
 * from the others.  This routine allows the caller to specify exactly
 * what must be removed through the use of the \p remflag argument.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[in]		entry	A pointer to a #smat_entry_t to be
 *				removed from the table.
 * \param[in]		remflag	A bitwise mask of #ST_REM_FIRST,
 *				#ST_REM_SECOND, #ST_REM_HASH, and
 *				#ST_REM_FREE indicating which portions
 *				of the removal logic must be executed.
 *
 * \return	The function returns zero if the requested operations
 *		were completed successfully.  A non-zero return value
 *		indicates a catastrophic failure condition, but is
 *		unlikely to occur.
 */
db_err_t _st_remove(smat_table_t *table, smat_entry_t *entry,
		    db_flag_t remflag);

/** \internal
 * \ingroup dbprim_smat
 * \brief Flag requesting removal from first list.
 *
 * This flag may be passed to _st_remove() to request that a sparse
 * matrix entry should be removed from the #SMAT_LOC_FIRST linked
 * list.
 */
#define ST_REM_FIRST	0x0001	/* remove from first list */

/** \internal
 * \ingroup dbprim_smat
 * \brief Flag requesting removal from second list.
 *
 * This flag may be passed to _st_remove() to request that a sparse
 * matrix entry should be removed from the #SMAT_LOC_SECOND linked
 * list.
 */
#define ST_REM_SECOND	0x0002	/* remove from second list */

/** \internal
 * \ingroup dbprim_smat
 * \brief Flag requesting removal from hash table.
 *
 * This flag may be passed to _st_remove() to request that a sparse
 * matrix entry should be removed from the hash table.
 */
#define ST_REM_HASH	0x0004	/* remove from hash table */

/** \internal
 * \ingroup dbprim_smat
 * \brief Flag requesting memory release.
 *
 * This flag may be passed to _st_remove() to request that a sparse
 * matrix entry be passed to _smat_free().
 */
#define ST_REM_FREE	0x0008	/* free the entry */

/** \internal
 * \ingroup dbprim_smat
 * \brief Allocate a sparse matrix entry.
 *
 * This function is used to allocate a sparse matrix entry.  In
 * cooperation with _smat_free(), it maintains a freelist in order to
 * reduce memory fragmentation.  If a sparse matrix entry cannot be
 * allocated from the freelist, a new one will be allocated with
 * malloc().
 *
 * \return	A pointer to an initialized sparse matrix entry.
 */
smat_entry_t *_smat_alloc(void);

/** \internal
 * \ingroup dbprim_smat
 * \brief Release a sparse matrix entry.
 *
 * This function is used to release a sparse matrix entry.  In
 * cooperatio with _smat_alloc(), it maintains a freelist in order to
 * reduce memory fragmentation.  Sparse matrix entries passed to this
 * function will generally be added to the free list.
 *
 * \param[in]		entry	The entry to release.
 */
void _smat_free(smat_entry_t *entry);

/** \internal
 * \ingroup dbprim_smat
 * \brief Sparse matrix resize function.
 *
 * This function is a hash table-compatible resize callback for use by
 * sparse matrices.
 *
 * \param[in]		table	The hash table being resized.
 * \param[in]		new_mod	The new hash table bucket size.
 *
 * \return	Zero if the resize operation should be performed,
 *		non-zero otherwise.
 */
db_err_t _smat_resize(hash_table_t *table, hash_t new_mod);

#endif /* included_dbprim_sparsemat_int_h__ */
