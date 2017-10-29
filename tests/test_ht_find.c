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

void
test_zerotable(void **state)
{
  db_err_t err;
  hash_entry_t *entry;
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_find(0, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t *entry;
  db_key_t key = DB_KEY_INIT("spam", 4);

  table.ht_magic = 0;

  err = ht_find(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zerokey(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t *entry;

  err = ht_find(&table, &entry, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_empty_table(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t *entry = 0;
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_find(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_NOENTRY);
  assert_ptr_equal(entry, 0);
}

void
test_empty_table_zeroentry(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ht_find(&table, 0, &key);

  assert_int_equal(err, DB_ERR_NOENTRY);
}

void
test_success(void **state)
{
  int i;
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t *entry;
  hash_entry_t entries[4] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0)
  };
  db_key_t key = DB_KEY_INIT("spam", 4);
  db_key_t entkeys[4] = {
    DB_KEY_INIT("aaaa", 4), DB_KEY_INIT("aaab", 4), DB_KEY_INIT("spam", 4),
    DB_KEY_INIT("aaad", 4)
  };

  /* Initialize the entries */
  for (i = 0; i < 4; i++) {
    entries[i].he_elem.le_object = &entries[i];
    entries[i].he_elem.le_next = i + 1 < 4 ? &entries[i + 1].he_elem : 0;
    entries[i].he_key = entkeys[i];
  }
  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_count = 4;
  table.ht_table[6].lh_first = &entries[0].he_elem;

  err = ht_find(&table, &entry, &key);

  assert_int_equal(err, 0);
  assert_ptr_equal(entry, &entries[2]);
}

void
test_success_zeroentry(void **state)
{
  int i;
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entries[4] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0)
  };
  db_key_t key = DB_KEY_INIT("spam", 4);
  db_key_t entkeys[4] = {
    DB_KEY_INIT("aaaa", 4), DB_KEY_INIT("aaab", 4), DB_KEY_INIT("spam", 4),
    DB_KEY_INIT("aaad", 4)
  };

  /* Initialize the entries */
  for (i = 0; i < 4; i++) {
    entries[i].he_elem.le_object = &entries[i];
    entries[i].he_elem.le_next = i + 1 < 4 ? &entries[i + 1].he_elem : 0;
    entries[i].he_key = entkeys[i];
  }
  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_count = 4;
  table.ht_table[6].lh_first = &entries[0].he_elem;

  err = ht_find(&table, 0, &key);

  assert_int_equal(err, 0);
}

void
test_missing(void **state)
{
  int i;
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t *entry = 0;
  hash_entry_t entries[4] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0)
  };
  db_key_t key = DB_KEY_INIT("spam", 4);
  db_key_t entkeys[4] = {
    DB_KEY_INIT("aaaa", 4), DB_KEY_INIT("aaab", 4), DB_KEY_INIT("aaac", 4),
    DB_KEY_INIT("aaad", 4)
  };

  /* Initialize the entries */
  for (i = 0; i < 4; i++) {
    entries[i].he_elem.le_object = &entries[i];
    entries[i].he_elem.le_next = i + 1 < 4 ? &entries[i + 1].he_elem : 0;
    entries[i].he_key = entkeys[i];
  }
  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_count = 4;
  table.ht_table[6].lh_first = &entries[0].he_elem;

  err = ht_find(&table, &entry, &key);

  assert_int_equal(err, DB_ERR_NOENTRY);
  assert_ptr_equal(entry, 0);
}

void
test_missing_zeroentry(void **state)
{
  int i;
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, hash_comp, 0, 0);
  hash_entry_t entries[4] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0)
  };
  db_key_t key = DB_KEY_INIT("spam", 4);
  db_key_t entkeys[4] = {
    DB_KEY_INIT("aaaa", 4), DB_KEY_INIT("aaab", 4), DB_KEY_INIT("aaac", 4),
    DB_KEY_INIT("aaad", 4)
  };

  /* Initialize the entries */
  for (i = 0; i < 4; i++) {
    entries[i].he_elem.le_object = &entries[i];
    entries[i].he_elem.le_next = i + 1 < 4 ? &entries[i + 1].he_elem : 0;
    entries[i].he_key = entkeys[i];
  }
  table.ht_table = linktab;
  table.ht_modulus = 7;
  table.ht_count = 4;
  table.ht_table[6].lh_first = &entries[0].he_elem;

  err = ht_find(&table, 0, &key);

  assert_int_equal(err, DB_ERR_NOENTRY);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerotable),
    cmocka_unit_test(test_badtable),
    cmocka_unit_test(test_zerokey),
    cmocka_unit_test(test_empty_table),
    cmocka_unit_test(test_empty_table_zeroentry),
    cmocka_unit_test(test_success),
    cmocka_unit_test(test_success_zeroentry),
    cmocka_unit_test(test_missing),
    cmocka_unit_test(test_missing_zeroentry)
  };

  return cmocka_run_group_tests_name("Test ht_find.c", tests, 0, 0);
}
