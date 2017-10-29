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
#include <cmocka.h>

#include "common_int.h"

void
test_enomem(void **state)
{
  const char *text;

  text = dbprim_err(ENOMEM);

  assert_null(text);
}

void
test_low(void **state)
{
  const char *text;

  text = dbprim_err(DB_ERR_BADARGS - 1);

  assert_null(text);
}

void
test_high(void **state)
{
  const char *text;

  text = dbprim_err(DB_ERR_UNRECOVERABLE + 1);

  assert_null(text);
}

void
test_badargs(void **state)
{
  const char *text;

  text = dbprim_err(DB_ERR_BADARGS);

  assert_non_null(text);
}

void
test_unrecoverable(void **state)
{
  const char *text;

  text = dbprim_err(DB_ERR_UNRECOVERABLE);

  assert_non_null(text);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_enomem),
    cmocka_unit_test(test_low),
    cmocka_unit_test(test_high),
    cmocka_unit_test(test_badargs),
    cmocka_unit_test(test_unrecoverable)
  };

  return cmocka_run_group_tests_name("Test dbprim_err.c", tests, 0, 0);
}
