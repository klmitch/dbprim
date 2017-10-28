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
#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <cmocka.h>

#include "mock_malloc.h"

/* Sentinel used to indicate that the fakes should be used */
static int _malloc_wrapper = 0;

/* Set up and tear down functions for utilizing the wrapper */
int
malloc_setup(void **state)
{
  _malloc_wrapper = 1;

  return 0;
}

int
malloc_teardown(void **state)
{
  _malloc_wrapper = 0;

  return 0;
}

/* The actual wrappers */
void *
__wrap_malloc(size_t size)
{
  void *ptr;

  /* Use the real malloc unless we're wanted */
  if (!_malloc_wrapper)
    return (void *)__real_malloc(size);

  check_expected(size);
  ptr = (void *)mock();
  if (!ptr)
    errno = (int)mock();

  return ptr;
}

void
__wrap_free(void *ptr)
{
  /* Use the real free unless we're wanted */
  if (!_malloc_wrapper) {
    __real_free(ptr);
    return;
  }

  check_expected(ptr);
}
