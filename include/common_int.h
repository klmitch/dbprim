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
#ifndef included_dbprim_common_int_h__
#define included_dbprim_common_int_h__

/** \internal
 * \file
 * \brief Common internal header file.
 *
 * This file contains the common internal definitions for the Database
 * Primitives library.
 */

#include "common.h"

#ifndef DBPRIM_DOXYGEN
# ifdef __GNUC__
#  if (__GNUC__ < 2) || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#   define __attribute__(A)
#  endif
# else
#  define __attribute__(A)
# endif
#endif /* DBPRIM_DOXYGEN */

#endif /* included_dbprim_common_int_h__ */
