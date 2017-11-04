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
__wrap_ht_find(hash_table_t *table, hash_entry_t **entry_p, db_key_t *key)
{
  check_expected_ptr(table);
  check_expected_ptr(entry_p);
  check_expected_ptr(key);
  return (db_err_t)mock();
}

db_err_t
__wrap_ll_remove(link_head_t *list, link_elem_t *elem)
{
  check_expected_ptr(list);
  check_expected_ptr(elem);
  return (db_err_t)mock();
}

db_err_t
__wrap_ll_add(link_head_t *list, link_elem_t *new, link_loc_t loc,
	      link_elem_t *elem)
{
  check_expected_ptr(list);
  check_expected_ptr(new);
  check_expected(loc);
  check_expected_ptr(elem);
  return (db_err_t)mock();
}

void
test_zerotable(void **state)
{
  db_err_t err;
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_move(0, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_magic = 0;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeroentry(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_move(&table, 0, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badentry(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  entry.he_magic = 0;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zerokey(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  err = ht_move(&table, &entry, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_unused(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_UNUSED);
}

void
test_wrongtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_table_t other = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  entry.he_table = &other;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_frozen(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  entry.he_table = &table;
  table.ht_flags |= HASH_FLAG_FREEZE;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_FROZEN);
}

void
test_duplicate(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  will_return(__wrap_ht_find, 0);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  entry.he_table = &table;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_DUPLICATE);
}

void
test_llremove_fails(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ll_remove, 42);
  expect_value(__wrap_ll_remove, list, &linktab[0]);
  expect_value(__wrap_ll_remove, elem, &entry.he_elem);
  table.ht_table = linktab;
  entry.he_table = &table;
  entry.he_hash = 0;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, 42);
}

void
test_readdfailed(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ll_remove, 0);
  expect_value(__wrap_ll_remove, list, &linktab[0]);
  expect_value(__wrap_ll_remove, elem, &entry.he_elem);
  will_return(__wrap_ll_add, 42);
  expect_value(__wrap_ll_add, list, &linktab[6]);
  expect_value(__wrap_ll_add, new, &entry.he_elem);
  expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
  expect_value(__wrap_ll_add, elem, 0);
  table.ht_table = linktab;
  table.ht_count = 5;
  table.ht_modulus = 7;
  entry.he_table = &table;
  entry.he_hash = 0;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_READDFAILED);
  assert_ptr_equal(entry.he_key.dk_key, key.dk_key);
  assert_int_equal(entry.he_key.dk_len, key.dk_len);
  assert_int_equal(entry.he_hash, 6);
  assert_int_equal(table.ht_count, 4);
  assert_ptr_equal(entry.he_table, 0);
}

void
test_happypath(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ll_remove, 0);
  expect_value(__wrap_ll_remove, list, &linktab[0]);
  expect_value(__wrap_ll_remove, elem, &entry.he_elem);
  will_return(__wrap_ll_add, 0);
  expect_value(__wrap_ll_add, list, &linktab[6]);
  expect_value(__wrap_ll_add, new, &entry.he_elem);
  expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
  expect_value(__wrap_ll_add, elem, 0);
  table.ht_table = linktab;
  table.ht_count = 5;
  table.ht_modulus = 7;
  entry.he_table = &table;
  entry.he_hash = 0;

  err = ht_move(&table, &entry, &key);

  assert_int_equal(err, 0);
  assert_ptr_equal(entry.he_key.dk_key, key.dk_key);
  assert_int_equal(entry.he_key.dk_len, key.dk_len);
  assert_int_equal(entry.he_hash, 6);
  assert_int_equal(table.ht_count, 5);
  assert_ptr_equal(entry.he_table, &table);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerotable),
    cmocka_unit_test(test_badtable),
    cmocka_unit_test(test_zeroentry),
    cmocka_unit_test(test_badentry),
    cmocka_unit_test(test_zerokey),
    cmocka_unit_test(test_unused),
    cmocka_unit_test(test_wrongtable),
    cmocka_unit_test(test_frozen),
    cmocka_unit_test(test_duplicate),
    cmocka_unit_test(test_llremove_fails),
    cmocka_unit_test(test_readdfailed),
    cmocka_unit_test(test_happypath)
  };

  return cmocka_run_group_tests_name("Test ht_move.c", tests, 0, 0);
}
