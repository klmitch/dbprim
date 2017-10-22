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
#ifndef included_dbprim_sparsemat_h__
#define included_dbprim_sparsemat_h__

/** \file
 * \brief Sparse matrices.
 *
 * This header file contains the definitions for the sparse matrices
 * component of the Database Primitives library.
 */

/** \defgroup dbprim_smat Sparse matrices
 * \brief Operations for sparse matrices.
 *
 * Sparse matrices are advanced data structures used to represent
 * associations.  For instance, a manager may have several employees,
 * but several of those employees may report to more than one
 * manager.  (Yes, this is a contrived example, so sue me.)  The
 * simplest way to represent such assocations is with a matrix, or a
 * two-dimensional array.  However, such an implementation cannot
 * easily be extended dynamically--imagine if a manager retires and
 * two more are hired, for instance.  It would also use an enormous
 * amount of memory, as most employees would only report to one or two
 * managers.
 *
 * A sparse matrix solves this problem by only allocating memory for
 * the cells in the full matrix which are actually used.  That is, no
 * memory is allocated to represent Alice reporting to Bob unless
 * Alice actually does report to Bob.  This is a simple concept, but
 * fairly difficult to implement efficiently--how do you tell if Alice
 * reports to Bob?  The solution utilized by this library is to
 * combine the strengths of linked lists and hash tables.  Each cell
 * is in two linked lists, rooted at the rows and columns of the
 * matrix, but a hash table is used when attempting to look up a given
 * cell.  If the cell is allocated, then there will be an entry in the
 * hash table, and finding the given cell is as fast as a hash table
 * look-up.
 *
 * Because sparse matrices are so complicated, there are three
 * structures and a variety of operations used.  Two of the
 * structures, #smat_table_t and #smat_head_t, are caller-allocated.
 * However, the third structure, #smat_entry_t, must be allocated by
 * the library.  To avoid too much overhead from malloc(), a free list
 * is used.  The free list may be managed with the smat_cleanup() and
 * smat_freemem() calls.
 *
 * The #smat_table_t contains the hash table.  Only one of these need
 * be allocated per type of association--for instance, in the above
 * example, only one #smat_table_t needs to be allocated to represent
 * the manager-employee relationship.
 *
 * The #smat_head_t contains the linked list.  There are actually two
 * kinds of these structures--one is #SMAT_LOC_FIRST, which could be
 * regarded as a ``row,'' and the other is #SMAT_LOC_SECOND, which
 * could be regarded as a ``column.''  Which one is used for which
 * type of data is irrelevant, as long as consistency is maintained.
 * For the above example, a #smat_head_t for a manager may be
 * #SMAT_LOC_FIRST, and one for an employee must then be
 * #SMAT_LOC_SECOND.  (These values are set when initializing the
 * #smat_head_t structure.)
 *
 * An association may be created with the st_add() function, which
 * allows an arbitrary ordering in the associated linked lists by the
 * same mechanism as for the linked list component of the library.  An
 * association may be removed with st_remove(), or looked up with
 * st_find().  If iteration over all associations is desired, use the
 * st_iter() function.  Removing all associations from a table may be
 * performed with st_flush(), which optionally calls a user-defined
 * clean-up function.  The associated hash table may be resized with
 * st_resize(), and the bucket table may be released with st_free().
 *
 * An association may also be reordered within the linked lists using
 * the sh_move() function.  If a particular entry is desired, use the
 * sh_find() function with a user-defined comparison function to
 * locate it.  Iteration may be performed with the sh_iter() function,
 * and all entries in a given linked list may be removed with the
 * sh_flush() function, which again may optionally call a user-defined
 * clean-up function.
 */

#ifndef __DBPRIM_LIBRARY__
#include <dbprim/common.h>
#include <dbprim/linklist.h>
#include <dbprim/hashtab.h>
#else
#include "common.h"
#include "linklist.h"
#include "hashtab.h"
#endif /* __DBPRIM_LIBRARY__ */

DBPRIM_BEGIN_C_DECLS

/** \ingroup dbprim_smat
 * \brief Sparse matrix table.
 *
 * This structure is the basis of all sparse matrices maintained by
 * this library.
 */
typedef struct _smat_table_s smat_table_t;

/** \ingroup dbprim_smat
 * \brief Sparse matrix list head.
 *
 * This structure is the head of a linked list of sparse matrix
 * entries.
 */
typedef struct _smat_head_s  smat_head_t;

/** \ingroup dbprim_smat
 * \brief Sparse matrix entry.
 *
 * This structure is allocated by the library and represents a single
 * element in a sparse matrix.
 */
typedef struct _smat_entry_s smat_entry_t;

/** \ingroup dbprim_smat
 * \brief Sparse matrix table resize callback.
 *
 * This function pointer references a callback that will be called
 * with both the old and new sparse matrix table sizes whenever a
 * sparse matrix's hash table table is resized.  It should return
 * non-zero only when the resize should be inhibited.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[in]		new_mod	The new table modulus.
 *
 * \return	Zero to permit the table resize, non-zero otherwise.
 */
typedef db_err_t (*smat_resize_t)(smat_table_t *table, hash_t new_mod);

/** \ingroup dbprim_smat
 * \brief Sparse matrix iteration callback.
 *
 * This function pointer references a callback used by st_iter(),
 * st_flush(), sh_iter(), and sh_flush().  It should return 0 for
 * success.  A non-zero return value will terminate the operation and
 * will become the return value of the call.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[in]		ent	A pointer to the #smat_entry_t being
 *				considered.
 * \param[in]		extra	A \c void pointer passed to st_iter(),
 *				st_flush(), sh_iter(), or sh_flush().
 *
 * \return	Zero for success, or non-zero to terminate the
 *		iteration.
 */
typedef db_err_t (*smat_iter_t)(smat_table_t *table, smat_entry_t *ent,
				void *extra);

/** \ingroup dbprim_smat
 * \brief Sparse matrix comparison callback.
 *
 * This function pointer references a callback used by sh_find().  It
 * should return 0 if the sparse matrix entry represented by the
 * second argument matches the key passed as the first argument.
 *
 * \param[in]		key	The database key being searched for.
 * \param[in]		ent	A pointer to the #smat_entry_t being
 *				considered.
 *
 * \return	Zero if \p key matches \p ent, non-zero otherwise.
 */
typedef int (*smat_comp_t)(db_key_t *key, smat_entry_t *ent);

/** \ingroup dbprim_smat
 * \brief Sparse matrix location.
 *
 * This enumeration is used to specify whether an element is a row or
 * column element.  It should be referenced by the typedef
 * #smat_loc_t.
 */
enum _smat_loc_e {
  SMAT_LOC_FIRST,	/**< First entry (``row''). */
  SMAT_LOC_SECOND	/**< Second entry (``column''). */
};

/** \ingroup dbprim_smat
 * \brief Sparse matrix location.
 *
 * See the documentation for the enumeration #_smat_loc_e.
 */
typedef enum _smat_loc_e smat_loc_t;

/** \ingroup dbprim_smat
 * \brief Clean up the smat free list.
 *
 * This function frees all smat_entry_t objects on the internal free
 * list.  It is always successful and returns 0.
 *
 * \return	This function always returns 0.
 */
db_err_t smat_cleanup(void);

/** \ingroup dbprim_smat
 * \brief Report how much memory is used by the free list.
 *
 * This function returns the amount of memory being used by the
 * internal free list of smat_entry_t objects.
 *
 * \return	A number indicating the size, in bytes, of the memory
 *		allocated for smat_entry_t objects on the free list.
 */
size_t smat_freemem(void);

/** \internal
 * \ingroup dbprim_smat
 * \brief Retrieve pointer to sparse matrix entry.
 *
 * This simple macro simply retrieves a pointer to the sparse matrix
 * entry from a linked list element.  It is a helper macro for the
 * other macros that access sparse matrix entries.
 *
 * \param[in]		ent	A pointer to a #link_elem_t.
 *
 * \return	A pointer to the relevant #smat_entry_t.
 */
#define _smat_ent(ent)	((smat_entry_t *)le_object(ent))

/** \internal
 * \ingroup dbprim_smat
 * \brief Sparse matrix table structure.
 *
 * This is the implementation of the #smat_table_t type.
 */
struct _smat_table_s {
  _db_magic_t	st_magic;	/**< Magic number. */
  smat_resize_t	st_resize;	/**< Function pointer for resize callback. */
  void	       *st_extra;	/**< Extra data pointer. */
  hash_table_t	st_table;	/**< Hash table. */
};

/** \internal
 * \ingroup dbprim_smat
 * \brief Sparse matrix table magic number.
 *
 * This is the magic number used for the sparse matrix table
 * structure.
 */
#define SMAT_TABLE_MAGIC 0x2f92a7b1

/** \ingroup dbprim_smat
 * \brief Sparse matrix table verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * sparse matrix table.
 *
 * \warning	This macro evaluates the \p table argument twice.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	Boolean \c true if \p table is a valid sparse matrix
 *		table or \c false otherwise.
 */
#define st_verify(table)	((table) && \
				 (table)->st_magic == SMAT_TABLE_MAGIC)

/** \ingroup dbprim_smat
 * \brief Sparse matrix table flags.
 *
 * This macro retrieves the flags associated with the sparse matrix
 * table.  Only #HASH_FLAG_AUTOGROW and #HASH_FLAG_AUTOSHRINK have any
 * meaning to the application; all other bits are reserved for use in
 * the library.  This macro may be used as an lvalue, but care must be
 * taken to avoid modifying the library-specific bits.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	A #db_flag_t containing the flags for the sparse
 *		matrix table.
 */
#define st_flags(table)	  ((table)->st_table.ht_flags)

/** \ingroup dbprim_smat
 * \brief Determine if a sparse matrix is frozen.
 *
 * This macro returns a non-zero value if the matrix is currently
 * frozen.  The sparse matrix may be frozen if there is an iteration
 * in progress.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	A zero value if the matrix is not frozen or a non-zero
 *		value if the matrix is frozen.
 */
#define st_frozen(table)  ((table)->st_table.ht_flags & HASH_FLAG_FREEZE)

/** \ingroup dbprim_smat
 * \brief Sparse matrix table modulus.
 *
 * This macro retrieves the number of buckets allocated for the sparse
 * matrix table.  An application may wish to save this value between
 * invocations to avoid the overhead of growing the table while
 * filling it with data.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	A #hash_t containing the number of buckets allocated
 *		for the sparse matrix table.
 */
#define st_modulus(table) ((table)->st_table.ht_modulus)

/** \ingroup dbprim_smat
 * \brief Sparse matrix table count.
 *
 * This macro retrieves the total number of items actually in the
 * sparse matrix table.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	A #db_count_t containing a count of the number of
 *		items in the sparse matrix table.
 */
#define st_count(table)	  ((table)->st_table.ht_count)

/** \ingroup dbprim_smat
 * \brief Sparse matrix table resize callback function.
 *
 * This macro retrieves the resize callback function pointer.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	A #smat_resize_t.
 */
#define st_rsize(table)	  ((table)->st_resize)

/** \ingroup dbprim_smat
 * \brief Extra pointer data in a sparse matrix table.
 *
 * This macro retrieves the extra pointer data associated with a
 * particular sparse matrix table.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	A pointer to \c void.
 */
#define st_extra(table)	  ((table)->st_extra)

/** \ingroup dbprim_smat
 * \brief Sparse matrix table memory size.
 *
 * This macro returns the physical size of the memory allocated by the
 * library for this sparse matrix table.
 *
 * \note The st_size() macro already counts the memory for each list
 * in the table.  Summing the results of sh_size() and st_size() will
 * over-count the amount of memory actually in use. 
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \return	A \c size_t.
 */
#define st_size(table) ((table)->st_table.ht_modulus * sizeof(link_head_t) + \
			(table)->st_table.ht_count * sizeof(smat_entry_t))

/** \ingroup dbprim_smat
 * \brief Dynamically initialize a sparse matrix table.
 *
 * This function initializes a sparse matrix table.
 *
 * \param[in]		table	A pointer to a #smat_table_t to be
 *				initialized.
 * \param[in]		flags	A bit-wise OR of #HASH_FLAG_AUTOGROW
 *				and #HASH_FLAG_AUTOSHRINK.  If neither
 *				behavior is desired, use 0.
 * \param[in]		resize	A #hash_resize_t function pointer for
 *				determining whether resizing is
 *				permitted and/or for notification of
 *				the resize.
 * \param[in]		extra	Extra pointer data that should be
 *				associated with the sparse matrix
 *				table.
 * \param[in]		init_mod
 *				An initial modulus for the table.
 *				This will presumably be extracted by
 *				st_modulus() in a previous invocation
 *				of the application.  A 0 value is
 *				valid.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval ENOMEM		Unable to allocate memory.
 */
db_err_t st_init(smat_table_t *table, db_flag_t flags, smat_resize_t resize,
		 void *extra, hash_t init_mod);

/** \ingroup dbprim_smat
 * \brief Add an entry to a sparse matrix.
 *
 * This function adds an entry to a sparse matrix.  The entry is
 * referenced in three different places, thus the complex set of
 * arguments.  This function will allocate a #smat_entry_t and return
 * it through the \c entry_p result parameter.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[out]		entry_p	A pointer to a pointer to a
 *				#smat_entry_t.  If \c NULL is passed,
 *				the addition will be performed and an
 *				appropriate error code returned.
 * \param[in]		head1	A pointer to a #smat_head_t
 * 				representing a #SMAT_LOC_FIRST sparse
 *				matrix list.
 * \param[in]		loc1	A #link_loc_t indicating where the
 *				entry should be added for \c head1.
 * \param[in]		ent1	A pointer to a #smat_entry_t
 *				describing another element in the list
 *				represented by \c head1 if \p loc1 is
 *				#LINK_LOC_BEFORE or #LINK_LOC_AFTER.
 * \param[in]		head2	A pointer to a #smat_head_t
 *				representing a #SMAT_LOC_SECOND sparse
 *				matrix list.
 * \param[in]		loc2	A #link_loc_t indicating where the
 *				entry should be added for \c head2.
 * \param[in]		ent2	A pointer to a #smat_entry_t
 *				describing another element in the list
 *				represented by \c head2 if \p loc2 is
 *				#LINK_LOC_BEFORE or #LINK_LOC_AFTER.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_BUSY		One of the arguments is already in the
 *				table.
 * \retval DB_ERR_FROZEN	The table is currently frozen.
 * \retval DB_ERR_NOTABLE	The bucket table has not been
 *				allocated and automatic growth is not
 *				enabled.
 * \retval DB_ERR_WRONGTABLE	One of the arguments was not in the
 *				proper table or list.
 * \retval DB_ERR_UNUSED	One of the \c ent arguments is not
 *				presently in a list.
 * \retval DB_ERR_UNRECOVERABLE	An unrecoverable error occurred while
 *				resizing the table.
 * \retval ENOMEM		No memory could be allocated for the
 *				#smat_entry_t structure.
 */
db_err_t st_add(smat_table_t *table, smat_entry_t **entry_p,
		smat_head_t *head1, link_loc_t loc1, smat_entry_t *ent1,
		smat_head_t *head2, link_loc_t loc2, smat_entry_t *ent2);

/** \ingroup dbprim_smat
 * \brief Remove an entry from a sparse matrix.
 *
 * This function removes the given entry from the specified sparse
 * matrix.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[in]		entry	A pointer to a #smat_entry_t to be
 *				removed from the table.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_WRONGTABLE	Entry is not in this sparse matrix.
 * \retval DB_ERR_UNRECOVERABLE	An unrecoverable error occurred while
 *				removing the entry from the table.
 */
db_err_t st_remove(smat_table_t *table, smat_entry_t *entry);

/** \ingroup dbprim_smat
 * \brief Find an entry in a sparse matrix.
 *
 * This function looks up the entry matching the given \p head1 and \p
 * head2.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[out]		entry_p	A pointer to a pointer to a
 *				#smat_entry_t.  If \c NULL is passed,
 *				the lookup will be performed and an
 *				appropriate error code returned.
 * \param[in]		head1	A pointer to a #smat_head_t
 *				initialized to #SMAT_LOC_FIRST.
 * \param[in]		head2	A pointer to a #smat_head_t
 *				initialized to #SMAT_LOC_SECOND.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	One or both of \p head1 or \p head2
 *				are not referenced in this table.
 * \retval DB_ERR_NOENTRY	No matching entry was found.
 */
db_err_t st_find(smat_table_t *table, smat_entry_t **entry_p,
		 smat_head_t *head1, smat_head_t *head2);

/** \ingroup dbprim_smat
 * \brief Iterate over each entry in a sparse matrix.
 *
 * This function iterates over every entry in a sparse matrix (in an
 * unspecified order), executing the given \p iter_func on each entry.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[in]		iter_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on
 *				an entry in a sparse matrix.  \c NULL
 *				is an invalid value.  See the
 *				documentation for #smat_iter_t for
 *				more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p iter_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The sparse matrix is frozen.
 */
db_err_t st_iter(smat_table_t *table, smat_iter_t iter_func, void *extra);

/** \ingroup dbprim_smat
 * \brief Flush a sparse matrix.
 *
 * This function flushes a sparse matrix--that is, it removes each
 * entry from the matrix.  If a \p flush_func is specified, it will be
 * called on the entry after it has been removed from the table, and
 * may safely call <CODE>free()</CODE>.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[in]		flush_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on
 *				an entry after removing it from the
 *				table.  May be \c NULL.  See the
 *				documentation for #smat_iter_t for
 *				more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p iter_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The sparse matrix is frozen.
 */
db_err_t st_flush(smat_table_t *table, smat_iter_t flush_func, void *extra);

/** \ingroup dbprim_smat
 * \brief Resize a sparse matrix table.
 *
 * This function resizes the hash table associated with a sparse
 * matrix based on the \p new_size parameter.  See the documentation
 * for ht_resize() for more information.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 * \param[in]		new_size
 *				A new size value for the table.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The table is currently frozen.
 * \retval DB_ERR_UNRECOVERABLE	A catastrophic error was encountered.
 *				The table is now unusable.
 * \retval ENOMEM		No memory could be allocated for the
 *				new bucket table.
 */
db_err_t st_resize(smat_table_t *table, hash_t new_size);

/** \ingroup dbprim_smat
 * \brief Free memory used by an empty sparse matrix table.
 *
 * This function releases the memory used by the bucket table of the
 * empty hash table associated with a sparse matrix.
 *
 * \param[in]		table	A pointer to a #smat_table_t.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_FROZEN	The table is frozen.
 * \retval DB_ERR_NOTEMPTY	The table is not empty.
 */
db_err_t st_free(smat_table_t *table);

/** \internal
 * \ingroup dbprim_smat
 * \brief Sparse matrix list head structure.
 *
 * This is the implementation of the #smat_head_t type.
 */
struct _smat_head_s {
  _db_magic_t	sh_magic;	/**< Magic number. */
  smat_loc_t	sh_elem;	/**< 0 or 1 to indicate first or second. */
  smat_table_t *sh_table;	/**< Table this object's in. */
  link_head_t	sh_head;	/**< Linked list head. */
};

/** \internal
 * \ingroup dbprim_smat
 * \brief Sparse matrix list head magic number.
 *
 * This is the magic number used for the sparse matrix list head
 * structure.
 */
#define SMAT_HEAD_MAGIC 0x4e5d9b8e

/** \ingroup dbprim_smat
 * \brief Sparse matrix list head static initializer.
 *
 * This macro statically initializes a #smat_head_t.
 *
 * \param[in]		elem	One of #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND specifing whether the
 *				object is a member of the set of rows
 *				or columns.
 * \param[in]		object	A pointer to \c void representing the
 *				object associated with the list head.
 */
#define SMAT_HEAD_INIT(elem, object) \
	{ SMAT_HEAD_MAGIC, (elem), 0, LINK_HEAD_INIT(object) }

/** \ingroup dbprim_smat
 * \brief Sparse matrix list head verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * sparse matrix head.
 *
 * \warning	This macro evaluates the \p head argument twice.
 *
 * \param[in]		head	A pointer to a #smat_head_t.
 *
 * \return	Boolean \c true if \p head is a valid sparse matrix
 *		head or \c false otherwise.
 */
#define sh_verify(head)		((head) && \
				 (head)->sh_magic == SMAT_HEAD_MAGIC)

/** \ingroup dbprim_smat
 * \brief Sparse matrix list head element macro.
 *
 * This macro retrieves the position indicator for the sparse matrix
 * head.  It will return one of #SMAT_LOC_FIRST or #SMAT_LOC_SECOND.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	An #smat_loc_t.
 */
#define sh_elem(head)	((head)->sh_elem)

/** \ingroup dbprim_smat
 * \brief Sparse matrix list head table pointer.
 *
 * If there are any elements in this sparse matrix list head, this
 * macro will retrieve a pointer to the table in which they reside.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A pointer to #smat_table_t.
 */
#define sh_table(head)	((head)->sh_table)

/** \ingroup dbprim_smat
 * \brief Determine if a sparse matrix is frozen.
 *
 * This macro returns a non-zero value if the matrix is currently
 * frozen.  The sparse matrix may be frozen if there is an iteration
 * in progress.
 *
 * \param[in]		head	A pointer to a #smat_head_t.
 *
 * \return	A zero value if the matrix is not frozen or a non-zero
 *		value if the matrix is frozen.
 */
#define sh_frozen(head) (st_frozen((head)->sh_table))

/** \ingroup dbprim_smat
 * \brief Sparse matrix list count.
 *
 * This macro retrieves the number of elements in the sparse matrix
 * list rooted at \p head.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A #db_count_t containing a count of the number of
 *		elements in the sparse matrix list.
 */
#define sh_count(head)	((head)->sh_head.lh_count)

/** \internal
 * \ingroup dbprim_smat
 * \brief Access the first element pointer in a #smat_head_t.
 *
 * This helper macro is used to directly access the first linked list
 * element of a #smat_head_t.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A pointer to a #link_elem_t indicating the first
 *		element in the list.
 */
#define _sh_first(head)	((head)->sh_head.lh_first)

/** \ingroup dbprim_smat
 * \brief First element in sparse matrix list.
 *
 * This macro retrieves a pointer to the #smat_entry_t for the first
 * element in the sparse matrix list.
 *
 * \warning	This macro evaluates the \p head argument twice.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A pointer to #smat_entry_t.
 */
#define sh_first(head)	(_sh_first(head) ? _smat_ent(_sh_first(head)) : 0)

/** \internal
 * \ingroup dbprim_smat
 * \brief Access the last element pointer in a #smat_head_t.
 *
 * This helper macro is used to directly access the last linked list
 * element of a #smat_head_t.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A pointer to a #link_elem_t indicating the last
 *		element in the list.
 */
#define _sh_last(head)	((head)->sh_head.lh_last)

/** \ingroup dbprim_smat
 * \brief Last element in sparse matrix list.
 *
 * This macro retrieves a pointer to the #smat_entry_t for the last
 * element in the sparse matrix list.
 *
 * \warning	This macro evaluates the \p head argument twice.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A pointer to #smat_entry_t.
 */
#define sh_last(head)	(_sh_last(head) ? _smat_ent(_sh_last(head)) : 0)

/** \ingroup dbprim_smat
 * \brief Object represented by a sparse matrix list head.
 *
 * This macro retrieves a pointer to the object referenced by the
 * sparse matrix list head.
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A pointer to \c void.
 */
#define sh_object(head)	((head)->sh_head.lh_extra)

/** \ingroup dbprim_smat
 * \brief Sparse matrix list memory size.
 *
 * This macro returns the physical size of the memory allocated by the
 * library for this sparse matrix list.
 *
 * \note The st_size() macro already counts the memory for each list
 * in the table.  Summing the results of sh_size() and st_size() will
 * over-count the amount of memory actually in use. 
 *
 * \param[in]		head	A pointer to #smat_head_t.
 *
 * \return	A \c size_t.
 */
#define sh_size(head)	((head)->sh_elem.lh_count * sizeof(smat_entry_t))

/** \ingroup dbprim_smat
 * \brief Dynamically initialize a sparse matrix row or column head.
 *
 * This function dynamically initializes a sparse matrix row or column
 * linked list head.  The \p elem argument specifies whether the
 * object is to be associated with a #SMAT_LOC_FIRST list or a
 * #SMAT_LOC_SECOND list.
 *
 * \param[in]		head	A pointer to a #smat_head_t to be
 *				initialized.
 * \param[in]		elem	Either #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND.
 * \param[in]		object	A pointer to the object containing the
 *				sparse matrix row or column head.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 */
db_err_t sh_init(smat_head_t *head, smat_loc_t elem, void *object);

/** \ingroup dbprim_smat
 * \brief Move an entry within a row or column list.
 *
 * This function allows the specified entry to be shifted within the
 * linked list describing the row or column.  It is very similar to
 * the ll_move() function.
 *
 * \param[in]		head	A pointer to a #smat_head_t.
 * \param[in]		elem	A pointer to the #smat_entry_t
 *				describing the entry to be moved.
 * \param[in]		loc	A #link_loc_t indicating where the
 *				entry should be moved to.
 * \param[in]		elem2	A pointer to a #smat_entry_t
 *				describing another entry in the list
 *				if \p loc is #LINK_LOC_BEFORE or
 *				#LINK_LOC_AFTER.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_BUSY		\p elem and \p elem2 are the same
 *				entry.
 * \retval DB_ERR_WRONGTABLE	\p elem or \p elem2 are in a different
 *				row or column.
 * \retval DB_ERR_UNUSED	\p elem or \p elem2 are not in any row
 *				or column.
 */
db_err_t sh_move(smat_head_t *head, smat_entry_t *elem, link_loc_t loc,
		 smat_entry_t *elem2);

/** \ingroup dbprim_smat
 * \brief Find an entry in a row or column of a sparse matrix.
 *
 * This function iterates through the given row or column of a
 * sparse matrix looking for an element that matches the given \p key.
 *
 * \param[in]		head	A pointer to a #smat_head_t.
 * \param[out]		elem_p	A pointer to a pointer to a
 *				#smat_entry_t.  \c NULL is an invalid
 *				value.
 * \param[in]		comp_func
 *				A pointer to a comparison function
 *				used to compare the key to a
 *				particular entry.  See the
 *				documentation for #smat_comp_t for
 *				more information.
 * \param[in]		start	A pointer to a #smat_entry_t
 *				describing where in the row or column
 *				to start.  If \c NULL is passed, the
 *				beginning of the row or column will be
 *				assumed.
 * \param[in]		key	A key to search for.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this row or column.
 * \retval DB_ERR_NOENTRY	No matching entry was found.
 */
db_err_t sh_find(smat_head_t *head, smat_entry_t **elem_p,
		 smat_comp_t comp_func, smat_entry_t *start, db_key_t *key);

/** \ingroup dbprim_smat
 * \brief Iterate over each entry in a row or column of a sparse
 * matrix.
 *
 * This function iterates over a row or column of a sparse matrix,
 * executing the given \p iter_func for each entry.
 *
 * \param[in]		head	A pointer to a #smat_head_t.
 * \param[in]		start	A pointer to a #smat_entry_t
 *				describing where in the row or column
 *				to start.  If \c NULL is passed, the
 *				beginning of the row or column will be
 *				assumed.
 * \param[in]		iter_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on
 *				an entry in a row or column of a
 *				sparse matrix.  \c NULL is an invalid
 *				value.  See the documentation for
 *				#smat_iter_t for more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p iter_func.
 * \param[in]		flags	If #DB_FLAG_REVERSE is given,
 *				iteration will be done from the end of
 *				the list backwards towards the head.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this row or column.
 */
db_err_t sh_iter(smat_head_t *head, smat_entry_t *start, smat_iter_t iter_func,
		 void *extra, db_flag_t flags);

/** \ingroup dbprim_smat
 * \brief Flush a row or column of a sparse matrix.
 *
 * This function flushes a sparse matrix row or column--that is, it
 * removes each element from that row or column.  If a \p flush_func
 * is specified, it will be called on the entry after it has been
 * removed from the row or column, and may safely call
 * <CODE>free()</CODE>.
 *
 * \param[in]		head	A pointer to a #smat_head_t.
 * \param[in]		flush_func
 *				A pointer to a callback function used
 *				to perform user-specifed actions on an
 *				entry after removing it from the row
 *				or column.  May be \c NULL.  See the
 *				documentation for #smat_iter_t for
 *				more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p flush_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 */
db_err_t sh_flush(smat_head_t *head, smat_iter_t flush_func, void *extra);

/** \internal
 * \ingroup dbprim_smat
 * \brief Sparse matrix entry structure.
 *
 * This is the implementation of the #smat_entry_t type.
 */
struct _smat_entry_s {
  _db_magic_t	se_magic;	/**< Magic number. */
  smat_table_t *se_table;	/**< Sparse matrix table. */
  hash_entry_t	se_hash;	/**< Hash table entry. */
  link_elem_t	se_link[2];	/**< Linked list elements. */
  void	       *se_object[2];	/**< Objects. */
};

/** \internal
 * \ingroup dbprim_smat
 * \brief Sparse matrix entry magic number.
 *
 * This is the magic number used for the sparse matrix entry
 * structure.
 */
#define SMAT_ENTRY_MAGIC 0x466b34b5

/** \ingroup dbprim_smat
 * \brief Sparse matrix entry verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * sparse matrix entry.
 *
 * \warning	This macro evaluates the \p entry argument twice.
 *
 * \param[in]		entry	A pointer to a #smat_entry_t.
 *
 * \return	Boolean \c true if \p entry is a valid sparse matrix
 *		entry or \c false otherwise.
 */
#define se_verify(entry)	((entry) && \
				 (entry)->se_magic == SMAT_ENTRY_MAGIC)

/** \ingroup dbprim_smat
 * \brief Sparse matrix entry table.
 *
 * This macro retrieves a pointer to the table that the sparse matrix
 * entry is in.
 *
 * \param[in]		entry	A pointer to a #smat_entry_t.
 *
 * \return	A pointer to a #smat_table_t.
 */
#define se_table(entry)	    ((entry)->se_table)

/** \ingroup dbprim_smat
 * \internal
 * \brief Sparse matrix entry linked list element.
 *
 * This macro provides access to the linked list element buried in the
 * sparse matrix entry for use by the free list routines.
 *
 * \param[in]		entry	A pointer to a #smat_entry_t.
 *
 * \return	A pointer to a #link_elem_t.
 */
#define _se_link(entry)	    (&((entry)->se_hash.he_elem))

/** \ingroup dbprim_smat
 * \brief Sparse matrix entry flags.
 *
 * This macro retrieves a set of user-defined flags associated with
 * the entry.  It may be used as an lvalue to set those flags.
 *
 * \param[in]		entry	A pointer to a #smat_entry_t.
 *
 * \return	A #db_flag_t containing the flags associated with the
 *		entry.
 */
#define se_flags(entry)     ((entry)->se_hash.he_elem.le_flags)

/** \ingroup dbprim_smat
 * \brief Sparse matrix table entry hash value.
 *
 * This macro retrieves the hash value of the given sparse matrix
 * entry.  If the sparse matrix hash been resized, this value may not
 * be the same as a previous value.
 *
 * \param[in]		entry	A pointer to a #smat_entry_t.
 *
 * \return	A #hash_t containing the hash code for the entry.
 */
#define se_hash(entry)	    ((entry)->se_hash.he_hash)

/** \internal
 * \ingroup dbprim_smat
 * \brief Access the next element pointer in a #smat_entry_t.
 *
 * This helper macro is used to directly access the next linked list
 * element in a specific sparse matrix linked list.
 *
 * \param[in]		entry	A pointer to #smat_entry_t.
 * \param[in]		n	One of #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND to specify which list
 *				thread is desired.
 *
 * \return	A pointer to a #link_elem_t indicating the next
 *		element in the list.
 */
#define _se_next(entry, n)  ((entry)->se_link[(n)].le_next)

/** \ingroup dbprim_smat
 * \brief Next element in sparse matrix list.
 *
 * This macro retrieves a pointer to the #link_elem_t for the next
 * element in the sparse matrix list.
 *
 * \warning	This macro evaluates the \p entry and \p n arguments
 *		twice.
 *
 * \param[in]		entry	A pointer to #smat_entry_t.
 * \param[in]		n	One of #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND to specify which list
 *				thread is desired.
 *
 * \return	A pointer to #smat_entry_t.
 */
#define se_next(entry, n)   (_se_next(entry, n) ? \
			     _smat_ent(_se_next(entry, n)) : 0)

/** \internal
 * \ingroup dbprim_smat
 * \brief Access the previous element pointer in a #smat_entry_t.
 *
 * This helper macro is used to directly access the previous linked
 * list element in a specific sparse matrix linked list.
 *
 * \param[in]		entry	A pointer to #smat_entry_t.
 * \param[in]		n	One of #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND to specify which list
 *				thread is desired.
 *
 * \return	A pointer to a #link_elem_t indicating the previous
 *		element in the list.
 */
#define _se_prev(entry, n)  ((entry)->se_link[(n)].le_prev)

/** \ingroup dbprim_smat
 * \brief Previous element in sparse matrix list.
 *
 * This macro retrieves a pointer to the #link_elem_t for the previous
 * element in the sparse matrix list.
 *
 * \warning	This macro evaluates the \p entry and \p n arguments
 *		twice.
 *
 * \param[in]		entry	A pointer to #smat_entry_t.
 * \param[in]		n	One of #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND to specify which list
 *				thread is desired.
 *
 * \return	A pointer to #smat_entry_t.
 */
#define se_prev(entry, n)   (_se_prev(entry, n) ? \
			     _smat_ent(_se_prev(entry, n)) : 0)

/** \ingroup dbprim_smat
 * \brief Flags associated with an entry in a sparse matrix list.
 *
 * This macro retrieves a set of user-defined flags associated with
 * the entry in a sparse matrix list.  It may be used as an lvalue to
 * set those flags.
 *
 * \param[in]		entry	A pointer to #smat_entry_t.
 * \param[in]		n	One of #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND to specify which list
 *				thread is desired.
 *
 * \return	A #db_flag_t containing the flags associated with the
 *		entry.
 */
#define se_lflags(entry, n) ((entry)->se_link[(n)].le_flags)

/** \ingroup dbprim_smat
 * \brief Object associated with an entry in a sparse matrix list.
 *
 * This macro retrieves a pointer to one of the objects represented by
 * the entry.  It may be used as an lvalue to change the object
 * pointed to.  Care should be taken when using this feature.
 *
 * \param[in]		entry	A pointer to #smat_entry_t.
 * \param[in]		n	One of #SMAT_LOC_FIRST or
 *				#SMAT_LOC_SECOND to specify which list
 *				thread is desired.
 *
 * \return	A pointer to \c void representing the object.
 */
#define se_object(entry, n) ((entry)->se_object[(n)])

DBPRIM_END_C_DECLS

#endif /* included_dbprim_sparsemat_h__ */
