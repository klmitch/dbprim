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

#include "hashtab_int.h"

#include "mock_malloc.h"

void
test_zerotable(void **state)
{
  db_err_t err;

  err = ht_free(0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  table.ht_magic = 0;

  err = ht_free(&table);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_frozen(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  table.ht_flags |= HASH_FLAG_FREEZE;

  err = ht_free(&table);

  assert_int_equal(err, DB_ERR_FROZEN);
}

void
test_notempty(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  table.ht_count = 1;

  err = ht_free(&table);

  assert_int_equal(err, DB_ERR_NOTEMPTY);
}

void
test_happypath(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  expect_value(__wrap_free, ptr, linktab);
  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_rollover = 9;
  table.ht_rollunder = 5;

  err = ht_free(&table);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_modulus, 0);
  assert_int_equal(table.ht_rollover, 0);
  assert_int_equal(table.ht_rollunder, 0);
  assert_ptr_equal(table.ht_table, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerotable),
    cmocka_unit_test(test_badtable),
    cmocka_unit_test(test_frozen),
    cmocka_unit_test(test_notempty),
    cmocka_unit_test_setup_teardown(test_happypath,
				    malloc_setup, malloc_teardown)
  };

  return cmocka_run_group_tests_name("Test ht_free.c", tests, 0, 0);
}
