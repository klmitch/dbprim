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
#ifndef included_dbprim_linklist_h__
#define included_dbprim_linklist_h__

/** \file dbprim/linklist.h
 * \brief Linked lists.
 *
 * This header file contains the definitions for the linked lists
 * component of the Database Primitives library.
 */

/** \defgroup dbprim_link Linked lists
 * \brief Operations for linked lists.
 *
 * Linked lists are a very basic data structure used in building
 * databases.  This library provides a simple yet powerful
 * implementation of generic linked lists, based on two
 * caller-allocated structures.  The #link_head_t structure describes
 * the head of a linked list and contains information regarding the
 * number of elements in the linked list as well as pointers
 * referencing the first and last elements in the list.  The
 * #link_elem_t structure describes a specific element in the linked
 * list and contains pointers referencing the next and previous
 * elements in the list, as well as a pointer to the object, a pointer
 * to the head of the linked list, and a set of user-specified flags.
 *
 * Elements may be added at any arbitrary location in the linked list
 * with ll_add(); moved to any other arbitrary location in the linked
 * list with ll_move(); or removed from the list with ll_remove().  In
 * addition, the user may search the list using a user-defined
 * comparison function with ll_find(); iterate over every element in
 * the list with ll_iter(); or remove all items from the list with
 * ll_flush(), optionally executing a user-specified clean-up
 * function.
 */

#ifndef __DBPRIM_LIBRARY__
#include <dbprim/common.h>
#else
#include "common.h"
#endif /* __DBPRIM_LIBRARY__ */

DBPRIM_BEGIN_C_DECLS

/** \ingroup dbprim_link
 * \brief Linked list head.
 *
 * This structure is the head of all linked lists maintained by this
 * library.
 */
typedef struct _link_head_s  link_head_t;

/** \ingroup dbprim_link
 * \brief Linked list element.
 *
 * This structure represents a single element of a linked list.
 */
typedef struct _link_elem_s  link_elem_t;

/** \ingroup dbprim_link
 * \brief Linked list iteration callback.
 *
 * This function pointer references a callback used by ll_iter() and
 * ll_flush().  It should return 0 for success.  A non-zero return
 * value will terminate the operation and will become the return value
 * of the ll_iter() or ll_flush() call.
 *
 * \param[in]		head	A pointer to a #link_head_t.
 * \param[in]		elem	A pointer to the #link_elem_t being
 *				considered.
 * \param[in]		extra	A \c void pointer passed to ll_iter()
 *				or ll_flush().
 *
 * \return	Zero for success, or non-zero to terminate the
 *		iteration.
 */
typedef db_err_t (*link_iter_t)(link_head_t *list, link_elem_t *elem,
				void *extra);

/** \ingroup dbprim_link
 * \brief Linked list comparison callback.
 *
 * This function pointer references a callback used by ll_find().  It
 * should return 0 if the entry passed as the second argument matches
 * the key passed as the first argument.
 *
 * \param[in]		key	The database key being searched for.
 * \param[in]		obj	The object to compare with the key.
 *
 * \return	Zero if \p key matches \p obj, non-zero otherwise.
 */
typedef int (*link_comp_t)(db_key_t *key, void *obj);

/** \ingroup dbprim_link
 * \brief Linked list location.
 *
 * This enumeration is used to specify where an element in a linked
 * list should be placed.  It should be referenced by the typedef
 * #link_loc_t.
 */
enum _link_loc_e {
  LINK_LOC_HEAD,	/**< Element should be inserted at head of list. */
  LINK_LOC_TAIL,	/**< Element should be inserted at tail of list. */
  LINK_LOC_BEFORE,	/**< Element should be inserted before specified
			     element. */
  LINK_LOC_AFTER	/**< Element should be inserted after specified
			     element. */
};

/** \ingroup dbprim_link
 * \brief Linked list location.
 *
 * See the documentation for the enumeration #_link_loc_e.
 */
typedef enum _link_loc_e link_loc_t;

/** \internal
 * \ingroup dbprim_link
 * \brief Linked list head structure.
 *
 * This is the implementation of the #link_head_t type.
 */
struct _link_head_s {
  _db_magic_t	lh_magic;	/**< Magic number. */
  db_count_t	lh_count;	/**< Number of entries in the linked list. */
  link_elem_t  *lh_first;	/**< First element in the list. */
  link_elem_t  *lh_last;	/**< Last element in the list. */
  void	       *lh_extra;	/**< Extra data associated with list. */
};

/** \internal
 * \ingroup dbprim_link
 * \brief Linked list head magic number.
 *
 * This is the magic number used for the linked list head structure.
 */
#define LINK_HEAD_MAGIC	0x4c6155d7

/** \ingroup dbprim_link
 * \brief Linked list head static initializer.
 *
 * This macro statically initializes a #link_head_t.
 *
 * \param[in]		extra	Extra pointer data that should be
 *				associated with the list head.
 */
#define LINK_HEAD_INIT(extra)	{ LINK_HEAD_MAGIC, 0, 0, 0, (extra) }

/** \ingroup dbprim_link
 * \brief Linked list head verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * linked list head.
 *
 * \warning	This macro evaluates the \p list argument twice.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 *
 * \return	Boolean \c true if \p list is a valid linked list head
 *		or \c false otherwise.
 */
#define ll_verify(list)		((list) && \
				 (list)->lh_magic == LINK_HEAD_MAGIC)

/** \ingroup dbprim_link
 * \brief Linked list count.
 *
 * This macro retrieves the number of elements in a linked list.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 *
 * \return	A #db_count_t containing a count of the number of
 *		elements in the linked list.
 */
#define ll_count(list)	((list)->lh_count)

/** \ingroup dbprim_link
 * \brief First element in linked list.
 *
 * This macro retrieves the first element in a linked list.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 *
 * \return	A pointer to a #link_elem_t.
 */
#define ll_first(list)	((list)->lh_first)

/** \ingroup dbprim_link
 * \brief Last element in a linked list.
 *
 * This macro retrieves the last element in a linked list.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 *
 * \return	A pointer to a #link_elem_t.
 */
#define ll_last(list)	((list)->lh_last)

/** \ingroup dbprim_link
 * \brief Extra pointer data in a linked list.
 *
 * This macro retrieves the extra pointer data associated with a
 * particular linked list.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 *
 * \return	A pointer to \c void.
 */
#define ll_extra(list)	((list)->lh_extra)

/** \ingroup dbprim_link
 * \brief Dynamically initialize a linked list head.
 *
 * This function dynamically initializes a linked list head.
 *
 * \param[in]		list	A pointer to a #link_head_t to be
 *				initialized.
 * \param[in]		extra	A pointer to \c void containing extra
 *				pointer data associated with the
 *				linked list.
 *
 * \retval DB_ERR_BADARGS	A \c NULL pointer was passed for \p
 *				list.
 */
db_err_t ll_init(link_head_t *list, void *extra);

/** \ingroup dbprim_link
 * \brief Add an element to a linked list.
 *
 * This function adds a given element to a specified linked list in
 * the specified location.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 * \param[in]		new	A pointer to the #link_elem_t to be
 *				added to the linked list.
 * \param[in]		loc	A #link_loc_t indicating where the
 *				entry should be added.
 * \param[in]		elem	A pointer to a #link_elem_t describing
 *				another element in the list if \p loc
 *				is #LINK_LOC_BEFORE or
 *				#LINK_LOC_AFTER.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_BUSY		The element is already in a list.
 * \retval DB_ERR_WRONGTABLE	\p elem is in a different list.
 * \retval DB_ERR_UNUSED	\p elem is not in any list.
 */
db_err_t ll_add(link_head_t *list, link_elem_t *new, link_loc_t loc,
		link_elem_t *elem);

/** \ingroup dbprim_link
 * \brief Move an element within a linked list.
 *
 * This function moves a specified element within the linked list.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 * \param[in]		elem	A pointer to the #link_elem_t
 *				describing the element to be moved.
 * \param[in]		loc	A #link_loc_t indicating where the
 *				entry should be moved to.
 * \param[in]		elem2	A pointer to a #link_elem_t describing
 *				another element in the list if \p loc
 *				is #LINK_LOC_BEFORE or
 *				#LINK_LOC_AFTER.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_BUSY		\p new and \p elem are the same
 *				element.
 * \retval DB_ERR_WRONGTABLE	\p new or \p elem are in a different
 *				list.
 * \retval DB_ERR_UNUSED	\p new or \p elem are not in any
 *				list.
 */
db_err_t ll_move(link_head_t *list, link_elem_t *elem, link_loc_t loc,
		 link_elem_t *elem2);

/** \ingroup dbprim_link
 * \brief Remove an element from a linked list.
 *
 * This function removes a specified element from a linked list.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 * \param[in]		elem	A pointer to the #link_elem_t
 *				describing the element to be removed.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_UNUSED	\p elem is not in a linked list.
 * \retval DB_ERR_WRONGTABLE	\p elem is not in this linked list.
 */
db_err_t ll_remove(link_head_t *list, link_elem_t *elem);

/** \ingroup dbprim_link
 * \brief Find an element in a linked list.
 *
 * This function iterates through a linked list looking for an element
 * that matches the given \p key.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 * \param[out]		elem_p	A pointer to a pointer to a
 *				#link_elem_t.  \c NULL is an invalid
 *				value.
 * \param[in]		comp_func
 *				A pointer to a comparison function
 *				used to compare the key to a
 *				particular element.  See the
 *				documentation for #link_comp_t for
 *				more information.
 * \param[in]		start	A pointer to a #link_elem_t describing
 *				where in the linked list to start.  If
 *				\c NULL is passed, the beginning of
 *				the list will be assumed.
 * \param[in]		key	A key to search for.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this linked list.
 * \retval DB_ERR_NOENTRY	No matching entry was found.
 */
db_err_t ll_find(link_head_t *list, link_elem_t **elem_p,
		 link_comp_t comp_func, link_elem_t *start, db_key_t *key);

/** \ingroup dbprim_link
 * \brief Iterate over each entry in a linked list.
 *
 * This function iterates over a linked list, executing the given \p
 * iter_func for each entry.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 * \param[in]		start	A pointer to a #link_elem_t describing
 *				where in the linked list to start.  If
 *				\c NULL is passed, the beginning of
 *				the list will be assumed.
 * \param[in]		iter_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on
 *				an element in a linked list.  \c NULL
 *				is an invalid value.  See the
 *				documentation for #link_iter_t for
 *				more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p iter_func.
 * \param[in]		flags	If #DB_FLAG_REVERSE is given,
 *				iteration will be done from the end of
 *				the list backwards towards the head.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this linked list.
 */
db_err_t ll_iter(link_head_t *list, link_elem_t *start, link_iter_t iter_func,
		 void *extra, db_flag_t flags);

/** \ingroup dbprim_link
 * \brief Flush a linked list.
 *
 * This function flushes a linked list--that is, it removes each
 * element from the list.  If a \p flush_func is specified, it will be
 * called on the entry after it has been removed from the list, and
 * may safely call <CODE>free()</CODE>.
 *
 * \param[in]		list	A pointer to a #link_head_t.
 * \param[in]		flush_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on
 *				an element after removing it from the
 *				list.  May be \c NULL.  See the
 *				documentation for #link_iter_t for
 *				more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p flush_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 */
db_err_t ll_flush(link_head_t *list, link_iter_t flush_func, void *extra);

/** \internal
 * \ingroup dbprim_link
 * \brief Linked list element structure.
 *
 * This is the implementation of the #link_elem_t type.
 */
struct _link_elem_s {
  _db_magic_t	le_magic;	/**< Magic number. */
  link_elem_t  *le_next;	/**< Next element in list. */
  link_elem_t  *le_prev;	/**< Previous element in list. */
  void	       *le_object;	/**< The object pointed to by this link. */
  link_head_t  *le_head;	/**< The head of the list. */
  db_flag_t	le_flags;	/**< Flags associated with this element. */
};

/** \internal
 * \ingroup dbprim_link
 * \brief Linked list element magic number.
 *
 * This is the magic number used for the linked list element
 * structure.
 */
#define LINK_ELEM_MAGIC	0x97cdf72a

/** \ingroup dbprim_link
 * \brief Linked list element static initializer.
 *
 * This macro statically initializes a #link_elem_t.
 *
 * \param[in]		obj	A pointer to \c void representing the
 *				object associated with the element.
 */
#define LINK_ELEM_INIT(obj) { LINK_ELEM_MAGIC, 0, 0, (obj), 0, 0 }

/** \ingroup dbprim_link
 * \brief Linked list element verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * linked list element.
 *
 * \warning	This macro evaluates the \p element argument twice.
 *
 * \param[in]		element	A pointer to a #link_elem_t.
 *
 * \return	Boolean \c true if \p element is a valid linked list
 *		element or \c false otherwise.
 */
#define le_verify(element)	((element) && \
				 (element)->le_magic == LINK_ELEM_MAGIC)

/** \ingroup dbprim_link
 * \brief Linked list element next pointer.
 *
 * This macro retrieves a pointer to the next element in the linked
 * list.
 *
 * \param[in]		elem	A pointer to a #link_elem_t.
 *
 * \return	A pointer to a #link_elem_t representing the next
 *		element in the linked list.
 */
#define le_next(elem)	((elem)->le_next)

/** \ingroup dbprim_link
 * \brief Linked list element previous pointer.
 *
 * This macro retrieves a pointer to the previous element in the
 * linked list.
 *
 * \param[in]		elem	A pointer to a #link_elem_t.
 *
 * \return	A pointer to a #link_elem_t representing the previous
 *		element in the linked list.
 */
#define le_prev(elem)	((elem)->le_prev)

/** \ingroup dbprim_link
 * \brief Linked list element object pointer.
 *
 * This macro retrieves a pointer to the object represented by the
 * element.  It may be used as an lvalue to change the object pointed
 * to.  Care should be taken when using this feature.
 *
 * \param[in]		elem	A pointer to a #link_elem_t.
 *
 * \return	A pointer to \c void representing the object
 *		associated with the linked list element.
 */
#define le_object(elem)	((elem)->le_object)

/** \ingroup dbprim_link
 * \brief Linked list element head pointer.
 *
 * This macro retrieves a pointer to the head of the linked list that
 * the element is in.
 *
 * \param[in]		elem	A pointer to a #link_elem_t.
 *
 * \return	A pointer to a #link_head_t representing the head of
 *		the linked list the element is in.
 */
#define le_head(elem)	((elem)->le_head)

/** \ingroup dbprim_link
 * \brief Linked list element flags.
 *
 * This macro retrieves a set of user-defined flags associated with
 * the element.  It may be used as an lvalue to set those flags.
 *
 * \param[in]		elem	A pointer to a #link_elem_t.
 *
 * \return	A #db_flag_t containing the flags associated with the
 *		element.
 */
#define le_flags(elem)	((elem)->le_flags)

/** \ingroup dbprim_link
 * \brief Dynamically initialize a linked list element.
 *
 * This function dynamically initializes a linked list element.
 *
 * \param[in]		elem	A pointer to a #link_elem_t to be
 *				initialized.
 * \param[in]		object	A pointer to \c void used to represent
 *				the object associated with the
 *				element.
 *
 * \retval DB_ERR_BADARGS	A \c NULL pointer was passed for \p
 *				elem or \p object.
 */
db_err_t le_init(link_elem_t *elem, void *object);

DBPRIM_END_C_DECLS

#endif /* included_dbprim_linklist_h__ */
