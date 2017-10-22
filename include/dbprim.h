/*
** Copyright (C) 2002, 2006, 2017 by Kevin L. Mitchell <klmitch@mit.edu>
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
#ifndef included_dbprim_h__
#define included_dbprim_h__
/** \mainpage Database Primitives Library
 *
 * This library contains a set of database primitives.  The primitives
 * defined by this library include a powerful linked list abstraction,
 * a hash table with optional automatic resizing, a powerful and
 * efficient sparse matrix implementation, and a red-black tree with
 * iterative traversals.  All of the necessary declarations for using
 * this library are found in the header file dbprim.h.  For more
 * information about the components of this library, see the module
 * list.
 */

/** \file
 * \brief Core Database Primitives header file.
 *
 * This header file is a convenience header that includes all the
 * components of the Database Primitives library.
 */

#include <dbprim/common.h>
#include <dbprim/hashtab.h>
#include <dbprim/linklist.h>
#include <dbprim/redblack.h>
#include <dbprim/sparsemat.h>

#endif /* included_dbprim_h__ */
