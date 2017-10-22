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
#ifndef included_dbprim_hashtab_h__
#define included_dbprim_hashtab_h__

/** \file dbprim/hashtab.h
 * \brief Hash tables.
 *
 * This header file contains the definitions for the hash tables
 * component of the Database Primitives library.
 */

/** \defgroup dbprim_hash Hash tables
 * \brief Operations for hash tables.
 *
 * Hash tables are a basic data structure used in building databases.
 * Hash tables provide a means of storing data such that an arbitrary
 * entry may be looked up efficiently.  This library implements a hash
 * table that may optionally grow and shrink to provide maximum
 * efficiency.  The implementation is with two kinds of
 * caller-allocated structures--a #hash_table_t structure that
 * describes the table and a #hash_entry_t structure for each entry in
 * the table.  The library allocates a bucket array which must be
 * released with the ht_free() function when the hash table has been
 * emptied.  Additionally, the hash table may be manually resized with
 * the ht_resize() function.
 *
 * Entries may be added to and removed from the table using the
 * ht_add() and ht_remove() functions.  Additionally, the key on a
 * given entry may be changed using the ht_move() function.  Of
 * course, any given entry may be looked up using the ht_find()
 * function, and ht_iter() will execute a user-defined function for
 * each entry in the hash table (in an unspecified order).  The
 * ht_flush() function will remove all entries from the hash table,
 * optionally executing a user-specified clean-up function.
 */

#ifndef __DBPRIM_LIBRARY__
#include <dbprim/common.h>
#include <dbprim/linklist.h>
#else
#include "common.h"
#include "linklist.h"
#endif /* __DBPRIM_LIBRARY__ */

DBPRIM_BEGIN_C_DECLS

/** \ingroup dbprim_hash
 * \brief Hash value.
 *
 * This type is used for hash values, and for values related to hash
 * values, such as the table modulus.  It is exactly 32 bits.
 */
typedef uint32_t hash_t;

/** \ingroup dbprim_hash
 * \brief Hash table.
 *
 * This structure is the basis of all hash tables maintained by this
 * library.
 */
typedef struct _hash_table_s hash_table_t;

/** \ingroup dbprim_hash
 * \brief Hash table entry.
 *
 * This structure represents a single entry of a hash table.
 */
typedef struct _hash_entry_s hash_entry_t;

/** \ingroup dbprim_hash
 * \brief Hash table iteration callback.
 *
 * This function pointer references a callback used by ht_iter() and
 * ht_flush().  It should return 0 for success.  A non-zero return
 * value will terminate the operation and will become the return value
 * of the ht_iter() or ht_flush() call.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		ent	A pointer to the #hash_entry_t being
 *				considered.
 * \param[in]		extra	A \c void pointer passed to ht_iter()
 *				or ht_flush().
 *
 * \return	Zero for success, or non-zero to terminate the
 *		iteration.
 */
typedef db_err_t (*hash_iter_t)(hash_table_t *table, hash_entry_t *ent,
				void *extra);

/** \ingroup dbprim_hash
 * \brief Hash function callback.
 *
 * This function is associated with a hash table, and is responsible
 * for generating a hash value.  The full 32-bit range of a #hash_t
 * should be used--do *not* reduce the hash value by the modulus of
 * the hash table.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		key	The database key to hash.
 *
 * \return	A 32-bit hash value for \p key.
 */
typedef hash_t (*hash_func_t)(hash_table_t *table, db_key_t *key);

/** \ingroup dbprim_hash
 * \brief Hash table comparison callback.
 *
 * This function pointer references a callback used to compare entries
 * in a hash table.  It should return 0 for identical entries and
 * non-zero otherwise.  No assumptions should be made about the order
 * in which the two keys are passed to this function.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		key1	The first key being compared.
 * \param[in]		key2	The second key being compared.
 *
 * \return	Zero if the keys match, non-zero otherwise.
 */
typedef int (*hash_comp_t)(hash_table_t *table, db_key_t *key1,
			   db_key_t *key2);

/** \ingroup dbprim_hash
 * \brief Hash table resize callback.
 *
 * This function pointer references a callback that will be called
 * with both the old and new hash table sizes whenever a hash table is
 * resized.  It should return non-zero only when the resize should be
 * inhibited.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		new_mod	The new table modulus.
 *
 * \return	Zero to permit the table resize, non-zero otherwise.
 */
typedef db_err_t (*hash_resize_t)(hash_table_t *table, hash_t new_mod);

/** \ingroup dbprim_hash
 * \brief FNV-1 hash function.
 *
 * This is a hash function, compatible with #hash_func_t, based around
 * the FNV-1 hash algorithm.  See
 * http://www.isthe.com/chongo/tech/comp/fnv/ for more information
 * about FNV-1.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		key	The database key to hash.
 *
 * \return	A 32-bit hash value for \p key.
 */
hash_t hash_fnv1(hash_table_t *table, db_key_t *key);

/** \ingroup dbprim_hash
 * \brief FNV-1a hash function.
 *
 * This is a hash function, compatible with #hash_func_t, based around
 * the FNV-1a hash algorithm.  See
 * http://www.isthe.com/chongo/tech/comp/fnv/ for more information
 * about FNV-1a.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		key	The database key to hash.
 *
 * \return	A 32-bit hash value for \p key.
 */
hash_t hash_fnv1a(hash_table_t *table, db_key_t *key);

/** \ingroup dbprim_hash
 * \brief Hash comparison function.
 *
 * This is a hash comparison function, compatible with #hash_comp_t,
 * based around memcmp().
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		key1	The first key being compared.
 * \param[in]		key2	The second key being compared.
 *
 * \return	Zero if the keys match, non-zero otherwise.
 */
int hash_comp(hash_table_t *table, db_key_t *key1, db_key_t *key2);

/** \internal
 * \ingroup dbprim_hash
 * \brief Hash table structure.
 *
 * This is the implementation of the #hash_table_t type.
 */
struct _hash_table_s {
  _db_magic_t	ht_magic;	/**< Magic number. */
  db_flag_t	ht_flags;	/**< Flags associated with the table. */
  hash_t	ht_modulus;	/**< Size (modulus) of the hash
				     table--must be prime. */
  db_count_t	ht_count;	/**< Number of elements in the table. */
  hash_t	ht_rollover;	/**< Size at which the table grows. */
  hash_t	ht_rollunder;	/**< Size at which the table shrinks. */
  link_head_t  *ht_table;	/**< Actual table entries. */
  hash_func_t	ht_func;	/**< Function for computing the hash. */
  hash_comp_t	ht_comp;	/**< Function for comparing hash keys. */
  hash_resize_t	ht_resize;	/**< Function for resize notify/inhibit. */
  void	       *ht_extra;	/**< Extra data associated with the table. */
};

/** \internal
 * \ingroup dbprim_hash
 * \brief Hash table magic number.
 *
 * This is the magic number used for the hash table structure.
 */
#define HASH_TABLE_MAGIC 0x2da7ffd9

/** \ingroup dbprim_hash
 * \brief Flag permitting a hash table to automatically grow.
 *
 * If passed in to #HASH_TABLE_INIT() or #ht_init(), allows the hash
 * table to grow automatically.
 */
#define HASH_FLAG_AUTOGROW   0x00000001 /* let table automatically grow */

/** \ingroup dbprim_hash
 * \brief Flag permitting a hash table to automatically shrink.
 *
 * If passed in to #HASH_TABLE_INIT() or #ht_init(), allows the hash
 * table to shrink automatically.
 */
#define HASH_FLAG_AUTOSHRINK 0x00000002 /* let table automatically shrink */

/** \ingroup dbprim_hash
 * \brief Hash table flags that may be set by the user.
 *
 * This flag mask may be used to obtain flags that the hash table user
 * may set on a hash table.
 */
#define HASH_FLAG_MASK	     (HASH_FLAG_AUTOGROW | HASH_FLAG_AUTOSHRINK)

/** \internal
 * \brief Flag indicating hash table is frozen.
 *
 * This flag, if set on a hash table, indicates that the table is
 * frozen and may not be modified.
 */
#define HASH_FLAG_FREEZE     0x80000000 /* hash table frozen */

/** \ingroup dbprim_hash
 * \brief Hash table static initializer.
 *
 * This macro statically initializes a #hash_table_t.
 *
 * \param[in]		flags	A bit-wise OR of #HASH_FLAG_AUTOGROW
 *				and #HASH_FLAG_AUTOSHRINK.  If neither
 *				behavior is desired, use 0.
 * \param[in]		func	A #hash_func_t function pointer for a
 *				hash function.
 * \param[in]		comp	A #hash_comp_t function pointer for a
 *				comparison function.
 * \param[in]		resize	A #hash_resize_t function pointer for
 *				determining whether resizing is
 *				permitted and/or for notification of
 *				the resize.
 * \param[in]		extra	Extra pointer data that should be
 *				associated with the hash table.
 */
#define HASH_TABLE_INIT(flags, func, comp, resize, extra) \
	{ HASH_TABLE_MAGIC, (flags) & HASH_FLAG_MASK, 0, 0, 0, 0, 0, \
	  (func), (comp), (resize), (extra) }

/** \ingroup dbprim_hash
 * \brief Hash table verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * hash table.
 *
 * \warning	This macro evaluates the \p table argument twice.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	Boolean \c true if \p table is a valid hash table or
 *		\c false otherwise.
 */
#define ht_verify(table)	((table) && \
				 (table)->ht_magic == HASH_TABLE_MAGIC)

/** \ingroup dbprim_hash
 * \brief Hash table flags.
 *
 * This macro retrieves the flags associated with the hash table.
 * Only #HASH_FLAG_AUTOGROW and #HASH_FLAG_AUTOSHRINK have any meaning
 * to the application; all other bits are reserved for use in the
 * library.  This macro may be used as an lvalue, but care must be
 * taken to avoid modifying the library-specific bits.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	An #db_flag_t containing the flags for the hash
 *		table.
 */
#define ht_flags(table)	  ((table)->ht_flags)

/** \ingroup dbprim_hash
 * \brief Determine if a hash table is frozen.
 *
 * This macro returns a non-zero value if the table is currently
 * frozen.  The hash table may be frozen if there is an iteration in
 * progress.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A zero value if the table is not frozen or a non-zero
 *		value if the table is frozen.
 */
#define ht_frozen(table)  ((table)->ht_flags & HASH_FLAG_FREEZE)

/** \ingroup dbprim_hash
 * \brief Hash table modulus.
 *
 * This macro retrieves the number of buckets allocated for the hash
 * table.  An application may wish to save this value between
 * invocations to avoid the overhead of growing the table while
 * filling it with data.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A #hash_t containing the number of buckets allocated
 *		for the hash table.
 */
#define ht_modulus(table) ((table)->ht_modulus)

/** \ingroup dbprim_hash
 * \brief Hash table count.
 *
 * This macro retrieves the total number of items actually in the hash
 * table.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A #db_count_t containing a count of the number of
 *		items in the hash table.
 */
#define ht_count(table)	  ((table)->ht_count)

/** \ingroup dbprim_hash
 * \brief Hash table hash function.
 *
 * This macro retrieves the hash function pointer.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A #hash_func_t.
 */
#define ht_func(table)	  ((table)->ht_func)

/** \ingroup dbprim_hash
 * \brief Hash table comparison function.
 *
 * This macro retrieves the comparison function pointer.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A #hash_comp_t.
 */
#define ht_comp(table)	  ((table)->ht_comp)

/** \ingroup dbprim_hash
 * \brief Hash table resize callback function.
 *
 * This macro retrieves the resize callback function pointer.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A #hash_resize_t.
 */
#define ht_rsize(table)	  ((table)->ht_resize)

/** \ingroup dbprim_hash
 * \brief Extra pointer data in a hash table.
 *
 * This macro retrieves the extra pointer data associated with a
 * particular hash table.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A pointer to \c void.
 */
#define ht_extra(table)	  ((table)->ht_extra)

/** \ingroup dbprim_hash
 * \brief Hash table memory size.
 *
 * This macro returns the physical size of the bucket array allocated
 * by the library for this hash table.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \return	A \c size_t.
 */
#define ht_size(table)	  ((table)->ht_modulus * sizeof(link_head_t))

/** \ingroup dbprim_hash
 * \brief Dynamically initialize a hash table.
 *
 * This function dynamically initializes a hash table.
 *
 * \param[in]		table	A pointer to a #hash_table_t to be
 *				initialized.
 * \param[in]		flags	A bit-wise OR of #HASH_FLAG_AUTOGROW
 *				and #HASH_FLAG_AUTOSHRINK.  If neither
 *				behavior is desired, use 0.
 * \param[in]		func	A #hash_func_t function pointer for a
 *				hash function.
 * \param[in]		comp	A #hash_comp_t function pointer for a
 *				comparison function.
 * \param[in]		resize	A #hash_resize_t function pointer for
 *				determining whether resizing is
 *				permitted and/or for notification of
 *				the resize.
 * \param[in]		extra	Extra pointer data that should be
 *				associated with the hash table.
 * \param[in]		init_mod
 *				An initial modulus for the table.
 *				This will presumably be extracted by
 *				ht_modulus() in a previous invocation
 *				of the application.  A 0 value is
 *				valid.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval ENOMEM		Unable to allocate memory.
 */
db_err_t ht_init(hash_table_t *table, db_flag_t flags, hash_func_t func,
		 hash_comp_t comp, hash_resize_t resize, void *extra,
		 hash_t init_mod);

/** \ingroup dbprim_hash
 * \brief Add an entry to a hash table.
 *
 * This function adds an entry to a hash table.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		entry	A pointer to a #hash_entry_t to be
 *				added to the table.
 * \param[in]		key	A pointer to a #db_key_t containing
 *				the key for the entry.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_BUSY		The entry is already in a table.
 * \retval DB_ERR_FROZEN	The table is currently frozen.
 * \retval DB_ERR_NOTABLE	The bucket table has not been
 *				allocated and automatic growth is not
 *				enabled.
 * \retval DB_ERR_DUPLICATE	The entry is a duplicate of an
 *				existing entry.
 * \retval DB_ERR_UNRECOVERABLE	An unrecoverable error occurred while
 *				resizing the table.
 */
db_err_t ht_add(hash_table_t *table, hash_entry_t *entry, db_key_t *key);

/** \ingroup dbprim_hash
 * \brief Move an entry in the hash table.
 *
 * This function moves an existing entry in the hash table to
 * correspond to the new key.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		entry	A pointer to a #hash_entry_t to be
 *				moved.  It must already be in the hash
 *				table.
 * \param[in]		key	A pointer to a #db_key_t describing
 *				the new key for the entry.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_UNUSED	Entry is not in a hash table.
 * \retval DB_ERR_WRONGTABLE	Entry is not in this hash table.
 * \retval DB_ERR_FROZEN	Hash table is frozen.
 * \retval DB_ERR_DUPLICATE	New key is a duplicate of an existing
 *				key.
 * \retval DB_ERR_READDFAILED	Unable to re-add entry to table.
 */
db_err_t ht_move(hash_table_t *table, hash_entry_t *entry, db_key_t *key);

/** \ingroup dbprim_hash
 * \brief Remove an element from a hash table.
 *
 * This function removes the given element from the specified hash
 * table.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		entry	A pointer to a #hash_entry_t to be
 *				removed from the table.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_UNUSED	Entry is not in a hash table.
 * \retval DB_ERR_WRONGTABLE	Entry is not in this hash table.
 * \retval DB_ERR_FROZEN	Hash table is frozen.
 * \retval DB_ERR_UNRECOVERABLE	An unrecoverable error occurred while
 *				resizing the table.
 */
db_err_t ht_remove(hash_table_t *table, hash_entry_t *entry);

/** \ingroup dbprim_hash
 * \brief Find an entry in a hash table.
 *
 * This function looks up an entry matching the given \p key.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[out]		entry_p	A pointer to a pointer to a
 *				#hash_entry_t.  If \c NULL is passed,
 *				the lookup will be performed and an
 *				appropriate error code returned.
 * \param[in]		key	A pointer to a #db_key_t describing
 *				the item to find.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_NOENTRY	No matching entry was found.
 */
db_err_t ht_find(hash_table_t *table, hash_entry_t **entry_p, db_key_t *key);

/** \ingroup dbprim_hash
 * \brief Iterate over each entry in a hash table.
 *
 * This function iterates over every entry in a hash table (in an
 * unspecified order), executing the given \p iter_func on each entry.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		iter_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on
 *				an entry in a hash table. \c NULL is
 *				an invalid value.  See the
 *				documentation for #hash_iter_t for
 *				more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p iter_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The hash table is frozen.
 */
db_err_t ht_iter(hash_table_t *table, hash_iter_t iter_func, void *extra);

/** \ingroup dbprim_hash
 * \brief Flush a hash table.
 *
 * This function flushes a hash table--that is, it removes each entry
 * from the table.  If a \p flush_func is specified, it will be called
 * on the entry after it has been removed from the table, and may
 * safely call <CODE>free()</CODE>.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 * \param[in]		flush_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on
 *				an entry after removing it from the
 *				table.  May be \c NULL.  See the
 *				documentation for #hash_iter_t for
 *				more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p flush_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The hash table is frozen.
 */
db_err_t ht_flush(hash_table_t *table, hash_iter_t flush_func, void *extra);

/** \ingroup dbprim_hash
 * \brief Resize a hash table.
 *
 * This function resizes a hash table to the given \p new_size.  If \p
 * new_size is 0, then an appropriate new size based on the current
 * number of items in the hash table will be selected.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
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
db_err_t ht_resize(hash_table_t *table, hash_t new_size);

/** \ingroup dbprim_hash
 * \brief Free memory used by an empty hash table.
 *
 * This function releases the memory used by the bucket table in an
 * empty hash table.
 *
 * \param[in]		table	A pointer to a #hash_table_t.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_FROZEN	The table is frozen.
 * \retval DB_ERR_NOTEMPTY	The table is not empty.
 */
db_err_t ht_free(hash_table_t *table);

/** \internal
 * \ingroup dbprim_hash
 * \brief Hash table entry structure.
 *
 * This is the implementation of the #hash_entry_t type.
 */
struct _hash_entry_s {
  _db_magic_t	he_magic;	/**< Magic number. */
  link_elem_t	he_elem;	/**< Link element. */
  hash_table_t *he_table;	/**< Hash table we're in. */
  hash_t	he_hash;	/**< Hash value. */
  db_key_t	he_key;		/**< Entry's key. */
  void	       *he_value;	/**< Actual entry. */
};

/** \internal
 * \ingroup dbprim_hash
 * \brief Hash table entry magic number.
 *
 * This is the magic number used for the hash table entry structure.
 */
#define HASH_ENTRY_MAGIC 0x35afaf51

/** \ingroup dbprim_hash
 * \brief Hash table entry static initializer.
 *
 * This macro statically initializes a #hash_entry_t.
 *
 * \param[in]		value	A pointer to \c void representing the
 *				object associated with the entry.
 */
#define HASH_ENTRY_INIT(value) \
      { HASH_ENTRY_MAGIC, LINK_ELEM_INIT(0), 0, 0, DB_KEY_INIT(0, 0), (value) }

/** \ingroup dbprim_hash
 * \brief Hash table entry verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * hash table entry.
 *
 * \warning	This macro evaluates the \p entry argument twice.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t.
 *
 * \return	Boolean \c true if \p entry is a valid hash table
 *		entry or \c false otherwise.
 */
#define he_verify(entry)	((entry) && \
				 (entry)->he_magic == HASH_ENTRY_MAGIC)

/** \ingroup dbprim_hash
 * \brief Hash table entry linked list element.
 *
 * This macro provides access to the linked list element buried in the
 * hash table entry.  It should *not* be used on entries currently in
 * a hash table.  The purpose of this macro is to allow an object
 * containing a hash table entry to be placed upon a free list.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t.
 *
 * \return	A pointer to a #link_elem_t.
 */
#define he_link(entry)	(&((entry)->he_elem))

/** \ingroup dbprim_hash
 * \brief Hash table entry flags.
 *
 * This macro retrieves a set of user-defined flags associated with
 * the entry.  It may be used as an lvalue to set those flags.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t.
 *
 * \return	A #db_flag_t containing the flags associated with the
 *		entry.
 */
#define he_flags(entry)	((entry)->he_elem.le_flags)

/** \ingroup dbprim_hash
 * \brief Hash table entry table pointer.
 *
 * This macro retrieves a pointer to the hash table the entry is in.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t.
 *
 * \return	A pointer to a #hash_table_t.
 */
#define he_table(entry)	((entry)->he_table)

/** \ingroup dbprim_hash
 * \brief Hash table entry hash value.
 *
 * This macro retrieves the hash value of the given hash entry.  If
 * the hash table has been resized, this value may not be the same as
 * a previous value.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t.
 *
 * \return	A #hash_t containing the hash code for the entry.
 */
#define he_hash(entry)	((entry)->he_hash)

/** \ingroup dbprim_hash
 * \brief Hash table entry key pointer.
 *
 * This macro retrieves the key associated with the hash table entry.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t.
 *
 * \return	A pointer to a #db_key_t.
 */
#define he_key(entry)	(&((entry)->he_key))

/** \ingroup dbprim_hash
 * \brief Hash table entry value pointer.
 *
 * This macro retrieves the value associated with the hash table
 * entry.  It may be treated as an lvalue to change that value.  Care
 * should be taken when using this option.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t.
 *
 * \return	A pointer to \c void representing the value associated
 *		with this entry.
 */
#define he_value(entry)	((entry)->he_value)

/** \ingroup dbprim_hash
 * \brief Dynamically initialize a hash table entry.
 *
 * This function dynamically initializes a hash table entry.
 *
 * \param[in]		entry	A pointer to a #hash_entry_t to be
 *				initialized.
 * \param[in]		value	A pointer to \c void which will be the
 *				value of the hash table entry.
 *
 * \retval DB_ERR_BADARGS	A \c NULL pointer was passed for \p
 *				entry.
 */
db_err_t he_init(hash_entry_t *entry, void *value);

DBPRIM_END_C_DECLS

#endif /* included_dbprim_hashtab_h__ */
