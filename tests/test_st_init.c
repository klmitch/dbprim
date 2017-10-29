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
smat_resize(smat_table_t *table, hash_t new_mod)
{
  return 0;
}

db_err_t
__wrap_ht_init(hash_table_t *table, db_flag_t flags, hash_func_t func,
	       hash_comp_t comp, hash_resize_t resize, void *extra,
	       hash_t init_mod)
{
  check_expected_ptr(table);
  check_expected(flags);
  check_expected(func);
  check_expected(comp);
  check_expected(resize);
  check_expected_ptr(extra);
  check_expected(init_mod);
  return (db_err_t)mock();
}

void
test_badargs(void **state)
{
  db_err_t err;

  err = st_init(0, 0, 0, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_ht_init_fails(void **state)
{
  db_err_t err;
  smat_table_t table;
  int spam;

  table.st_magic = 0;
  will_return(__wrap_ht_init, 42);
  expect_value(__wrap_ht_init, table, &table.st_table);
  expect_value(__wrap_ht_init, flags, 1234);
  expect_value(__wrap_ht_init, func, hash_fnv1a);
  expect_value(__wrap_ht_init, comp, hash_comp);
  expect_value(__wrap_ht_init, resize, _smat_resize);
  expect_value(__wrap_ht_init, extra, &table);
  expect_value(__wrap_ht_init, init_mod, 6);

  err = st_init(&table, 1234, smat_resize, &spam, 6);

  assert_int_equal(err, 42);
  assert_int_equal(table.st_magic, 0);
  assert_ptr_equal(table.st_extra, &spam);
  assert_ptr_equal(table.st_resize, smat_resize);
}

void
test_success(void **state)
{
  db_err_t err;
  smat_table_t table;
  int spam;

  table.st_magic = 0;
  will_return(__wrap_ht_init, 0);
  expect_value(__wrap_ht_init, table, &table.st_table);
  expect_value(__wrap_ht_init, flags, 1234);
  expect_value(__wrap_ht_init, func, hash_fnv1a);
  expect_value(__wrap_ht_init, comp, hash_comp);
  expect_value(__wrap_ht_init, resize, _smat_resize);
  expect_value(__wrap_ht_init, extra, &table);
  expect_value(__wrap_ht_init, init_mod, 6);

  err = st_init(&table, 1234, smat_resize, &spam, 6);

  assert_int_equal(err, 0);
  assert_int_equal(table.st_magic, SMAT_TABLE_MAGIC);
  assert_ptr_equal(table.st_extra, &spam);
  assert_ptr_equal(table.st_resize, smat_resize);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badargs),
    cmocka_unit_test(test_ht_init_fails),
    cmocka_unit_test(test_success)
  };

  return cmocka_run_group_tests_name("Test st_init.c", tests, 0, 0);
}
