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
#ifndef included_mock_malloc_h__
#define included_mock_malloc_h__

#include <stdlib.h>

/* Setup and teardown functions */
int malloc_setup(void **state);
int malloc_teardown(void **state);

/* The real malloc() and free() */
void *__real_malloc(size_t size);
void __real_free(void *ptr);

#endif /* included_mock_malloc_h__ */
