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
__wrap_ll_add(link_head_t *list, link_elem_t *new, link_loc_t loc,
	      link_elem_t *elem)
{
  check_expected_ptr(list);
  check_expected_ptr(new);
  check_expected(loc);
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
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_add(0, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_magic = 0;

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeroentry(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_add(&table, 0, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badentry(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  entry.he_magic = 0;

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zerokey(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);

  err = ht_add(&table, &entry, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_busy(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  entry.he_table = &table;

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_BUSY);
}

void
test_frozen(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_flags |= HASH_FLAG_FREEZE;

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_FROZEN);
}

void
test_notable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_NOTABLE);
}

void
test_duplicate(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOGROW,
				       hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  will_return(__wrap_ht_find, 0);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_DUPLICATE);
}

void
test_bad_resize(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOGROW,
				       hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ht_resize, 42);
  expect_value(__wrap_ht_resize, table, &table);
  expect_value(__wrap_ht_resize, new_size, 1);

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, 42);
}

void
test_badlink(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOGROW,
				       hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_rollover = 9;
  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ll_add, 42);
  expect_value(__wrap_ll_add, list, &table.ht_table[6]);
  expect_value(__wrap_ll_add, new, &entry.he_elem);
  expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
  expect_value(__wrap_ll_add, elem, 0);

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, 42);
  assert_ptr_equal(entry.he_elem.le_object, &entry);
  assert_ptr_equal(entry.he_key.dk_key, key.dk_key);
  assert_int_equal(entry.he_key.dk_len, key.dk_len);
  assert_int_equal(entry.he_hash, 6);
  assert_int_equal(table.ht_count, 0);
  assert_ptr_equal(entry.he_table, 0);
}

void
test_base(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOGROW,
				       hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_rollover = 9;
  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ll_add, 0);
  expect_value(__wrap_ll_add, list, &table.ht_table[6]);
  expect_value(__wrap_ll_add, new, &entry.he_elem);
  expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
  expect_value(__wrap_ll_add, elem, 0);

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, 0);
  assert_ptr_equal(entry.he_elem.le_object, &entry);
  assert_ptr_equal(entry.he_key.dk_key, key.dk_key);
  assert_int_equal(entry.he_key.dk_len, key.dk_len);
  assert_int_equal(entry.he_hash, 6);
  assert_int_equal(table.ht_count, 1);
  assert_ptr_equal(entry.he_table, &table);
}

void
test_will_resize(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOGROW,
				       hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_rollover = 9;
  table.ht_count = 9;
  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ht_resize, 0);
  expect_value(__wrap_ht_resize, table, &table);
  expect_value(__wrap_ht_resize, new_size, 13);
  will_return(__wrap_ll_add, 0);
  expect_value(__wrap_ll_add, list, &table.ht_table[6]);
  expect_value(__wrap_ll_add, new, &entry.he_elem);
  expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
  expect_value(__wrap_ll_add, elem, 0);

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, 0);
  assert_ptr_equal(entry.he_elem.le_object, &entry);
  assert_ptr_equal(entry.he_key.dk_key, key.dk_key);
  assert_int_equal(entry.he_key.dk_len, key.dk_len);
  assert_int_equal(entry.he_hash, 6);
  assert_int_equal(table.ht_count, 10);
  assert_ptr_equal(entry.he_table, &table);
}

void
test_willnot_resize(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entry = HASH_ENTRY_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_rollover = 9;
  table.ht_count = 9;
  will_return(__wrap_ht_find, DB_ERR_NOENTRY);
  expect_value(__wrap_ht_find, table, &table);
  expect_value(__wrap_ht_find, entry_p, 0);
  expect_value(__wrap_ht_find, key, &key);
  will_return(__wrap_ll_add, 0);
  expect_value(__wrap_ll_add, list, &table.ht_table[6]);
  expect_value(__wrap_ll_add, new, &entry.he_elem);
  expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
  expect_value(__wrap_ll_add, elem, 0);

  err = ht_add(&table, &entry, &key);

  assert_int_equal(err, 0);
  assert_ptr_equal(entry.he_elem.le_object, &entry);
  assert_ptr_equal(entry.he_key.dk_key, key.dk_key);
  assert_int_equal(entry.he_key.dk_len, key.dk_len);
  assert_int_equal(entry.he_hash, 6);
  assert_int_equal(table.ht_count, 10);
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
    cmocka_unit_test(test_busy),
    cmocka_unit_test(test_frozen),
    cmocka_unit_test(test_notable),
    cmocka_unit_test(test_duplicate),
    cmocka_unit_test(test_bad_resize),
    cmocka_unit_test(test_badlink),
    cmocka_unit_test(test_base),
    cmocka_unit_test(test_will_resize),
    cmocka_unit_test(test_willnot_resize)
  };

  return cmocka_run_group_tests_name("Test ht_add.c", tests, 0, 0);
}
