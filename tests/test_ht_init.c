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

#include "hashtab_int.h"

#include "mock_malloc.h"

/* Callback functions for testing */
hash_t
hash_func(hash_table_t *table, db_key_t *key)
{
  return 0;
}

int
hash_comp(hash_table_t *table, db_key_t *key1, db_key_t *key2)
{
  return 0;
}

db_err_t
hash_resize(hash_table_t *table, hash_t new_mod)
{
  return 0;
}

hash_t
__wrap__hash_prime(hash_t start)
{
  check_expected(start);
  return (hash_t)mock();
}

db_err_t
__wrap_ll_init(link_head_t *list, void *extra)
{
  check_expected_ptr(list);
  check_expected_ptr(extra);
  return (db_err_t)mock();
}

void
test_badtable(void **state)
{
  db_err_t err;

  err = ht_init(0, 0, hash_func, hash_comp, hash_resize, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badfunc(void **state)
{
  db_err_t err;
  hash_table_t table;

  err = ht_init(&table, 0, 0, hash_comp, hash_resize, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badcomp(void **state)
{
  db_err_t err;
  hash_table_t table;

  err = ht_init(&table, 0, hash_func, 0, hash_resize, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_empty(void **state)
{
  db_err_t err;
  hash_table_t table;
  int spam;

  will_return(__wrap__hash_prime, 0);
  expect_value(__wrap__hash_prime, start, 0);

  err = ht_init(&table, 0, hash_func, hash_comp, hash_resize, &spam, 0);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_magic, HASH_TABLE_MAGIC);
  assert_int_equal(table.ht_flags, 0);
  assert_int_equal(table.ht_modulus, 0);
  assert_int_equal(table.ht_count, 0);
  assert_int_equal(table.ht_rollover, 0);
  assert_int_equal(table.ht_rollunder, 0);
  assert_ptr_equal(table.ht_table, 0);
  assert_ptr_equal(table.ht_func, hash_func);
  assert_ptr_equal(table.ht_comp, hash_comp);
  assert_ptr_equal(table.ht_resize, hash_resize);
  assert_ptr_equal(table.ht_extra, &spam);
}

void
test_flagfilter(void **state)
{
  db_err_t err;
  hash_table_t table;
  int spam;

  will_return(__wrap__hash_prime, 0);
  expect_value(__wrap__hash_prime, start, 0);

  err = ht_init(&table, ~0, hash_func, hash_comp, hash_resize, &spam, 0);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_magic, HASH_TABLE_MAGIC);
  assert_int_equal(table.ht_flags, HASH_FLAG_AUTOGROW | HASH_FLAG_AUTOSHRINK);
  assert_int_equal(table.ht_modulus, 0);
  assert_int_equal(table.ht_count, 0);
  assert_int_equal(table.ht_rollover, 0);
  assert_int_equal(table.ht_rollunder, 0);
  assert_ptr_equal(table.ht_table, 0);
  assert_ptr_equal(table.ht_func, hash_func);
  assert_ptr_equal(table.ht_comp, hash_comp);
  assert_ptr_equal(table.ht_resize, hash_resize);
  assert_ptr_equal(table.ht_extra, &spam);
}

void
test_outofmemory(void **state)
{
  db_err_t err;
  hash_table_t table;
  int spam;

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 6);
  will_return(__wrap_malloc, 0);
  will_return(__wrap_malloc, ENOMEM);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));

  err = ht_init(&table, 0, hash_func, hash_comp, hash_resize, &spam, 6);

  assert_int_equal(err, ENOMEM);
}

void
test_ll_init_fails(void **state)
{
  db_err_t err;
  hash_table_t table;
  int spam;
  link_head_t linktab[7];

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 6);
  will_return(__wrap_malloc, linktab);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));
  will_return(__wrap_ll_init, 42);
  expect_value(__wrap_ll_init, list, linktab);
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_free, ptr, linktab);

  err = ht_init(&table, 0, hash_func, hash_comp, hash_resize, &spam, 6);

  assert_int_equal(err, 42);
}

void
test_ll_init_succeeds(void **state)
{
  db_err_t err;
  hash_table_t table;
  int spam;
  link_head_t linktab[7];

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 6);
  will_return(__wrap_malloc, linktab);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));
  will_return_count(__wrap_ll_init, 0, 7);
  expect_value(__wrap_ll_init, list, &linktab[0]); /* i = 0 */
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_ll_init, list, &linktab[1]); /* i = 1 */
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_ll_init, list, &linktab[2]); /* i = 2 */
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_ll_init, list, &linktab[3]); /* i = 3 */
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_ll_init, list, &linktab[4]); /* i = 4 */
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_ll_init, list, &linktab[5]); /* i = 5 */
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_ll_init, list, &linktab[6]); /* i = 6 */
  expect_value(__wrap_ll_init, extra, &table);

  err = ht_init(&table, 0, hash_func, hash_comp, hash_resize, &spam, 6);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_magic, HASH_TABLE_MAGIC);
  assert_int_equal(table.ht_flags, 0);
  assert_int_equal(table.ht_modulus, 7);
  assert_int_equal(table.ht_count, 0);
  assert_int_equal(table.ht_rollover, 9);
  assert_int_equal(table.ht_rollunder, 5);
  assert_ptr_equal(table.ht_table, linktab);
  assert_ptr_equal(table.ht_func, hash_func);
  assert_ptr_equal(table.ht_comp, hash_comp);
  assert_ptr_equal(table.ht_resize, hash_resize);
  assert_ptr_equal(table.ht_extra, &spam);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badtable),
    cmocka_unit_test(test_badfunc),
    cmocka_unit_test(test_badcomp),
    cmocka_unit_test(test_empty),
    cmocka_unit_test(test_flagfilter),
    cmocka_unit_test_setup_teardown(test_outofmemory,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_ll_init_fails,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_ll_init_succeeds,
				    malloc_setup, malloc_teardown)
  };

  return cmocka_run_group_tests_name("Test ht_init.c", tests, 0, 0);
}
