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
#ifndef included_dbprim_common_h__
#define included_dbprim_common_h__

/** \file
 * \brief Common header file.
 *
 * This header file contains the common definitions for the Database
 * Primitives library, such as including common system header files
 * and defining types used throughout the library.
 */

/** \defgroup dbprim Database Primitives
 *
 * This module describes interfaces common to all database
 * modules--mainly the macros concerned with manipulating database
 * keys and the definition of the key structure.
 *
 * The key may be any arbitrary pointer, including a pointer to a
 * string.  Everything that handles a key either copies the contents
 * of the #db_key_t structure or passes it to a user-defined
 * function.  If required, as in the case of a string, a length may
 * also be represented in the key structure.
 */

/** \internal
 * \def DBPRIM_BEGIN_C_DECLS
 * \brief Begin declaration in C namespace.
 *
 * This macro is defined to <CODE>extern "C" {</CODE> when compiling
 * with a C++ compiler.  See #DBPRIM_END_C_DECLS.
 */

/** \internal
 * \def DBPRIM_END_C_DECLS
 * \brief End declaration in C namespace.
 *
 * This macro is defined to <CODE>}</CODE> when compiling with a C++
 * compiler.  See #DBPRIM_BEGIN_C_DECLS.
 */
#undef DBPRIM_BEGIN_C_DECLS
#undef DBPRIM_END_C_DECLS
#ifdef __cplusplus
# define DBPRIM_BEGIN_C_DECLS extern "C" {
# define DBPRIM_END_C_DECLS }
#else
# define DBPRIM_BEGIN_C_DECLS /* empty */
# define DBPRIM_END_C_DECLS   /* empty */
#endif

DBPRIM_BEGIN_C_DECLS

#include <stdint.h>
#include <stdlib.h>	/* for size_t */

/* For the library version information */
#ifndef __DBPRIM_LIBRARY__
#include <dbprim/dbprim_version.h>
#endif /* __DBPRIM_LIBRARY__ */

/** \ingroup dbprim
 * \brief Error codes.
 *
 * This type is an alias for the error code types.
 */
typedef uint32_t db_err_t;

/** \internal
 * \ingroup dbprim
 * \brief Magic number type.
 *
 * This type is used for magic numbers on library structures.  Magic
 * numbers provide some protection against passing incorrect pointers.
 */
typedef uint32_t _db_magic_t;

/** \ingroup dbprim
 * \brief Flags.
 *
 * The behavior of several functions in the library may be controlled
 * through flags, either passed to the function or part of the
 * structures being manipulated.  Several structures also contain flag
 * fields that are for the use of the application.  This type provides
 * an alias for these flag fields.
 */
typedef uint32_t db_flag_t;

/** \ingroup dbprim
 * \brief Counts.
 *
 * This type is used exclusively to hold item counts.
 */
typedef uint32_t db_count_t;

/** \ingroup dbprim
 * \brief Database key.
 *
 * This structure is a generic key containing a void * pointer and a
 * length parameter.  It should be accessed with dk_key() and
 * dk_len().
 */
typedef struct _db_key_s db_key_t;

/** \internal
 * \ingroup dbprim
 * \brief Database key structure.
 *
 * This is the implementation of the #db_key_t type.
 */
struct _db_key_s {
  void	       *dk_key;	/**< Pointer to the key. */
  size_t	dk_len;	/**< Length of the key, if that has any meaning. */
};

/** \ingroup dbprim
 * \brief Database key static initializer.
 *
 * This macro allows a #db_key_t to be initialized statically.
 *
 * \param[in]		key	A pointer to the key.
 * \param[in]		size	Size of the key.
 */
#define DB_KEY_INIT(key, size) { (key), (size) }

/** \ingroup dbprim
 * \brief Database key accessor macro.
 *
 * This macro allows access to the key field of a #db_key_t.  It may
 * be used as an lvalue in order to assign a key to a #db_key_t.
 *
 * \param[in]		key	A pointer to a #db_key_t.
 *
 * \return	A pointer to a key (<CODE>void *</CODE>).
 */
#define dk_key(key)	((key)->dk_key)

/** \ingroup dbprim
 * \brief Database key length accessor macro.
 *
 * This macro allows access to the key length field of a #db_key_t.
 * It may be used as an lvalue in order to assign a length to a
 * #db_key_t.
 *
 * \param[in]		key	A pointer to a #db_key_t.
 *
 * \return	A \c size_t describing the length of the key.
 */
#define dk_len(key)	((key)->dk_len)

/** \ingroup dbprim
 * \brief Reverse flag.
 *
 * This flag can be passed to ordered iterations to reverse the
 * order of the iterations.
 */
#define DB_FLAG_REVERSE		0x80000000

/** \ingroup dbprim
 * \brief Bad arguments.
 *
 * This potential error return indicates that the provided arguments
 * are incorrect.
 */
#define DB_ERR_BADARGS		UINT32_C(2043325440)

/** \ingroup dbprim
 * \brief Element already in use.
 *
 * This potential error return indicates that the element to be added
 * already exists in a container.
 */
#define DB_ERR_BUSY		UINT32_C(2043325441)

/** \ingroup dbprim
 * \brief Element is not in use.
 *
 * This potential error return indicates that the element referenced
 * is not in a container.
 */
#define DB_ERR_UNUSED		UINT32_C(2043325442)

/** \ingroup dbprim
 * \brief Element is not in this table.
 *
 * This potential error return indicates that the element referenced
 * is not in the designated container.
 */
#define DB_ERR_WRONGTABLE	UINT32_C(2043325443)

/** \ingroup dbprim
 * \brief No matching element.
 *
 * This potential error return indicates that no element matching the
 * specified criteria was found.
 */
#define DB_ERR_NOENTRY		UINT32_C(2043325444)

/** \ingroup dbprim
 * \brief Duplicate keys found.
 *
 * This potential error return indicates that the key of the element
 * to be added already exists in the designated container.
 */
#define DB_ERR_DUPLICATE	UINT32_C(2043325445)

/** \ingroup dbprim
 * \brief Table not empty.
 *
 * This potential error return indicates that a container is not empty
 * and cannot have its allocated memory released.
 */
#define DB_ERR_NOTEMPTY		UINT32_C(2043325446)

/** \ingroup dbprim
 * \brief Table is frozen; try again later.
 *
 * This potential error return indicates that a container is frozen.
 * This typically occurs if the container is being iterated over; the
 * container will be frozen to ensure that the iteration order cannot
 * be altered.
 */
#define DB_ERR_FROZEN		UINT32_C(2043325447)

/** \ingroup dbprim
 * \brief No table allocated.
 *
 * This potential error return indicates that a container has no table
 * allocated and that automatic allocation has been disabled.
 */
#define DB_ERR_NOTABLE		UINT32_C(2043325448)

/** \ingroup dbprim
 * \brief Failed to re-add entry to table.
 *
 * This potential error return indicates that an entry which has been
 * moved (re-keyed) could not be re-added to the container with the
 * new key.
 */
#define DB_ERR_READDFAILED	UINT32_C(2043325449)

/** \ingroup dbprim
 * \brief Unrecoverable error; table unusable.
 *
 * This potential error return indicates that the container has been
 * corrupted beyond repair; the table must be discarded and rebuilt.
 */
#define DB_ERR_UNRECOVERABLE	UINT32_C(2043325450)

/** \ingroup dbprim
 * \brief Retrieve a string describing an error return code.
 *
 * This function retrieves a string that describes an error return
 * code returned by the library.
 *
 * \param[in]		err	The error code.
 *
 * \return	A constant string containing a description of the
 *		error, or \c NULL if there is no corresponding textual
 *		description of the error.
 */
const char *dbprim_err(db_err_t err);

DBPRIM_END_C_DECLS

#endif /* included_dbprim_common_h__ */
