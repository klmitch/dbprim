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

db_err_t
__wrap_ll_remove(link_head_t *list, link_elem_t *elem)
{
  check_expected_ptr(list);
  check_expected_ptr(elem);
  return (db_err_t)mock();
}

db_err_t
__wrap_ht_resize(hash_table_t *table, hash_t new_size)
{
  check_expected_ptr(table);
  check_expected(new_size);
  return (db_err_t)mock();
}

void
test_zerotable(void **state)
{
  db_err_t err;
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  err = ht_remove(0, &entry);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  table.ht_magic = 0;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeroentry(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  err = ht_remove(&table, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badentry(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  entry.he_magic = 0;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_unused(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  err = ht_remove(&table, &entry);

  assert_int_equal(err, DB_ERR_UNUSED);
}

void
test_wrongtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_table_t other = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  entry.he_table = &other;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_frozen(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  table.ht_flags |= HASH_FLAG_FREEZE;
  entry.he_table = &table;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, DB_ERR_FROZEN);
}

void
test_autoshrink_fails(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOSHRINK, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  will_return(__wrap_ht_resize, 42);
  expect_value(__wrap_ht_resize, table, &table);
  expect_value(__wrap_ht_resize, new_size, 4);
  table.ht_count = 5;
  table.ht_rollunder = 5;
  entry.he_table = &table;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, 42);
}

void
test_autoshrink_succeeds(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOSHRINK, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  will_return(__wrap_ht_resize, 0);
  expect_value(__wrap_ht_resize, table, &table);
  expect_value(__wrap_ht_resize, new_size, 4);
  will_return(__wrap_ll_remove, 0);
  expect_value(__wrap_ll_remove, list, &linktab[6]);
  expect_value(__wrap_ll_remove, elem, &entry.he_elem);
  table.ht_table = linktab;
  table.ht_count = 5;
  table.ht_rollunder = 5;
  entry.he_table = &table;
  entry.he_hash = 6;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, 0);
  assert_ptr_equal(entry.he_table, 0);
  assert_int_equal(table.ht_count, 4);
}

void
test_no_autoshrink(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  will_return(__wrap_ll_remove, 0);
  expect_value(__wrap_ll_remove, list, &linktab[6]);
  expect_value(__wrap_ll_remove, elem, &entry.he_elem);
  table.ht_table = linktab;
  table.ht_count = 5;
  table.ht_rollunder = 5;
  entry.he_table = &table;
  entry.he_hash = 6;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, 0);
  assert_ptr_equal(entry.he_table, 0);
  assert_int_equal(table.ht_count, 4);
}

void
test_llremove_fails(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  will_return(__wrap_ll_remove, 42);
  expect_value(__wrap_ll_remove, list, &linktab[6]);
  expect_value(__wrap_ll_remove, elem, &entry.he_elem);
  table.ht_table = linktab;
  table.ht_count = 5;
  table.ht_rollunder = 5;
  entry.he_table = &table;
  entry.he_hash = 6;

  err = ht_remove(&table, &entry);

  assert_int_equal(err, 42);
  assert_ptr_equal(entry.he_table, &table);
  assert_int_equal(table.ht_count, 5);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerotable),
    cmocka_unit_test(test_badtable),
    cmocka_unit_test(test_zeroentry),
    cmocka_unit_test(test_badentry),
    cmocka_unit_test(test_unused),
    cmocka_unit_test(test_wrongtable),
    cmocka_unit_test(test_frozen),
    cmocka_unit_test(test_autoshrink_fails),
    cmocka_unit_test(test_autoshrink_succeeds),
    cmocka_unit_test(test_no_autoshrink),
    cmocka_unit_test(test_llremove_fails)
  };

  return cmocka_run_group_tests_name("Test ht_remove.c", tests, 0, 0);
}
