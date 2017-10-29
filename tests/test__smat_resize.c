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
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "sparsemat_int.h"

db_err_t
test_smat_resize(smat_table_t *table, hash_t new_mod)
{
  check_expected_ptr(table);
  check_expected(new_mod);
  return (db_err_t)mock();
}

void
test_noresize(void **state)
{
  db_err_t err;
  hash_table_t hashtab;
  smat_table_t smattab;

  hashtab.ht_extra = &smattab;
  smattab.st_resize = 0;

  err = _smat_resize(&hashtab, 7);

  assert_int_equal(err, 0);
}

void
test_withresize(void **state)
{
  db_err_t err;
  hash_table_t hashtab;
  smat_table_t smattab;

  hashtab.ht_extra = &smattab;
  smattab.st_resize = test_smat_resize;
  will_return(test_smat_resize, 42);
  expect_value(test_smat_resize, table, &smattab);
  expect_value(test_smat_resize, new_mod, 7);

  err = _smat_resize(&hashtab, 7);

  assert_int_equal(err, 42);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_noresize),
    cmocka_unit_test(test_withresize)
  };

  return cmocka_run_group_tests_name("Test _smat_resize.c", tests, 0, 0);
}
