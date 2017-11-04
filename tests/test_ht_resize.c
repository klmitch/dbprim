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
__wrap_ll_remove(link_head_t *list, link_elem_t *elem)
{
  check_expected_ptr(list);
  check_expected_ptr(elem);

  assert_ptr_equal(list->lh_first, elem);

  list->lh_first = elem->le_next;

  return (db_err_t)mock();
}

db_err_t
resize_callback(hash_table_t *table, hash_t new_mod)
{
  check_expected_ptr(table);
  check_expected(new_mod);
  return (db_err_t)mock();
}

void
test_zerotable(void **state)
{
  db_err_t err;

  err = ht_resize(0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  table.ht_magic = 0;

  err = ht_resize(&table, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_frozen(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  table.ht_flags |= HASH_FLAG_FREEZE;

  err = ht_resize(&table, 0);

  assert_int_equal(err, DB_ERR_FROZEN);
}

void
test_zero_size(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, resize_callback, 0);

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 1);
  will_return(resize_callback, 42);
  expect_value(resize_callback, table, &table);
  expect_value(resize_callback, new_mod, 7);

  err = ht_resize(&table, 0);

  assert_int_equal(err, 42);
}

void
test_nonzero_size(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, resize_callback, 0);

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 6);
  will_return(resize_callback, 42);
  expect_value(resize_callback, table, &table);
  expect_value(resize_callback, new_mod, 7);
  table.ht_count = 5;

  err = ht_resize(&table, 0);

  assert_int_equal(err, 42);
}

void
test_explicit_size(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, resize_callback, 0);

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(resize_callback, 42);
  expect_value(resize_callback, table, &table);
  expect_value(resize_callback, new_mod, 7);

  err = ht_resize(&table, 5);

  assert_int_equal(err, 42);
}

void
test_resize_zero_malloc_fails(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, resize_callback, 0);

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(resize_callback, 0);
  expect_value(resize_callback, table, &table);
  expect_value(resize_callback, new_mod, 7);
  will_return(__wrap_malloc, 0);
  will_return(__wrap_malloc, ENOMEM);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));

  err = ht_resize(&table, 5);

  assert_int_equal(err, ENOMEM);
}

void
test_no_resize_malloc_fails(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(__wrap_malloc, 0);
  will_return(__wrap_malloc, ENOMEM);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));

  err = ht_resize(&table, 5);

  assert_int_equal(err, ENOMEM);
}

void
test_llinit_fails(void **state)
{
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(__wrap_malloc, linktab);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));
  will_return(__wrap_ll_init, 0);
  expect_value(__wrap_ll_init, list, &linktab[0]);
  expect_value(__wrap_ll_init, extra, &table);
  will_return(__wrap_ll_init, 42);
  expect_value(__wrap_ll_init, list, &linktab[1]);
  expect_value(__wrap_ll_init, extra, &table);
  expect_value(__wrap_free, ptr, linktab);

  err = ht_resize(&table, 5);

  assert_int_equal(err, 42);
}

void
test_empty(void **state)
{
  int i;
  db_err_t err;
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(__wrap_malloc, linktab);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));
  for (i = 0; i < 7; i++) {
    will_return(__wrap_ll_init, 0);
    expect_value(__wrap_ll_init, list, &linktab[i]);
    expect_value(__wrap_ll_init, extra, &table);
  }

  err = ht_resize(&table, 5);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_modulus, 7);
  assert_int_equal(table.ht_rollover, 9);
  assert_int_equal(table.ht_rollunder, 5);
  assert_int_equal(table.ht_table, linktab);
}

void
test_rehash(void **state)
{
  int i, j;
  db_err_t err;
  link_head_t oldtab[] = {
    LINK_HEAD_INIT(0), LINK_HEAD_INIT(0), LINK_HEAD_INIT(0)
  };
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entries[] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0)
  };
  db_key_t entry_keys[] = {
    DB_KEY_INIT("aaaa", 4), DB_KEY_INIT("aaab", 4), DB_KEY_INIT("aaac", 4),
    DB_KEY_INIT("aaad", 4), DB_KEY_INIT("aaae", 4), DB_KEY_INIT("aaaf", 4),
    DB_KEY_INIT("aaag", 4), DB_KEY_INIT("aaah", 4), DB_KEY_INIT("aaai", 4)
  };
#define hash_offset 5

  table.ht_table = oldtab;
  table.ht_modulus = 3;
  table.ht_count = 9;
  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(__wrap_malloc, linktab);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));
  for (i = 0; i < 7; i++) {
    will_return(__wrap_ll_init, 0);
    expect_value(__wrap_ll_init, list, &linktab[i]);
    expect_value(__wrap_ll_init, extra, &table);
  }
  for (i = 0; i < 3; i++) {
    /* Point link heads to the correct entries */
    oldtab[i].lh_first = &entries[i * 3].he_elem;

    /* Construct the testing linked list */
    for (j = 0; j < 3; j++) {
      entries[i * 3 + j].he_elem.le_object = &entries[i * 3 + j];
      entries[i * 3 + j].he_key = entry_keys[i * 3 + j];
      entries[i * 3 + j].he_elem.le_next =
	j + 1 < 3 ? &entries[i * 3 + j + 1].he_elem : 0;

      /* Set up the callbacks while we're here */
      will_return(__wrap_ll_remove, 0);
      expect_value(__wrap_ll_remove, list, &oldtab[i]);
      expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
      will_return(__wrap_ll_add, 0);
      expect_value(__wrap_ll_add, list,
		   &linktab[(i * 3 + j + hash_offset) % 7]);
      expect_value(__wrap_ll_add, new, &entries[i * 3 + j].he_elem);
      expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
      expect_value(__wrap_ll_add, elem, 0);
    }
  }
  expect_value(__wrap_free, ptr, oldtab);

  err = ht_resize(&table, 5);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_modulus, 7);
  assert_int_equal(table.ht_rollover, 9);
  assert_int_equal(table.ht_rollunder, 5);
  assert_int_equal(table.ht_table, linktab);
  for (i = 0; i < 9; i++) {
    assert_int_equal(entries[i].he_hash, (i + hash_offset) % 7);
  }
}

void
test_llremove_fails(void **state)
{
  int i, j;
  db_err_t err;
  link_head_t oldtab[] = {
    LINK_HEAD_INIT(0), LINK_HEAD_INIT(0), LINK_HEAD_INIT(0)
  };
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entries[] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0)
  };
  db_key_t entry_keys[] = {
    DB_KEY_INIT("aaaa", 4), DB_KEY_INIT("aaab", 4), DB_KEY_INIT("aaac", 4),
    DB_KEY_INIT("aaad", 4), DB_KEY_INIT("aaae", 4), DB_KEY_INIT("aaaf", 4),
    DB_KEY_INIT("aaag", 4), DB_KEY_INIT("aaah", 4), DB_KEY_INIT("aaai", 4)
  };
#define hash_offset 5

  table.ht_table = oldtab;
  table.ht_modulus = 3;
  table.ht_count = 9;
  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(__wrap_malloc, linktab);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));
  for (i = 0; i < 7; i++) {
    will_return(__wrap_ll_init, 0);
    expect_value(__wrap_ll_init, list, &linktab[i]);
    expect_value(__wrap_ll_init, extra, &table);
  }
  for (i = 0; i < 3; i++) {
    /* Point link heads to the correct entries */
    oldtab[i].lh_first = &entries[i * 3].he_elem;

    /* Construct the testing linked list */
    for (j = 0; j < 3; j++) {
      entries[i * 3 + j].he_elem.le_object = &entries[i * 3 + j];
      entries[i * 3 + j].he_key = entry_keys[i * 3 + j];
      entries[i * 3 + j].he_elem.le_next =
	j + 1 < 3 ? &entries[i * 3 + j + 1].he_elem : 0;

      /* Set up the callbacks while we're here */
      if (i * 3 + j < 5) {
	will_return(__wrap_ll_remove, 0);
	expect_value(__wrap_ll_remove, list, &oldtab[i]);
	expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
	will_return(__wrap_ll_add, 0);
	expect_value(__wrap_ll_add, list,
		     &linktab[(i * 3 + j + hash_offset) % 7]);
	expect_value(__wrap_ll_add, new, &entries[i * 3 + j].he_elem);
	expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
	expect_value(__wrap_ll_add, elem, 0);
      } else if (i * 3 + j == 5) {
	will_return(__wrap_ll_remove, 42);
	expect_value(__wrap_ll_remove, list, &oldtab[i]);
	expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
      }
    }
  }
  expect_value(__wrap_free, ptr, linktab);
  expect_value(__wrap_free, ptr, oldtab);

  err = ht_resize(&table, 5);

  assert_int_equal(err, DB_ERR_UNRECOVERABLE);
  assert_int_equal(table.ht_modulus, 0);
  assert_int_equal(table.ht_count, 0);
  assert_int_equal(table.ht_rollover, 0);
  assert_int_equal(table.ht_rollunder, 0);
  assert_int_equal(table.ht_table, 0);
}

void
test_lladd_fails(void **state)
{
  int i, j;
  db_err_t err;
  link_head_t oldtab[] = {
    LINK_HEAD_INIT(0), LINK_HEAD_INIT(0), LINK_HEAD_INIT(0)
  };
  link_head_t linktab[7];
  hash_table_t table = HASH_TABLE_INIT(0, hash_fnv1, 0, 0, 0);
  hash_entry_t entries[] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0)
  };
  db_key_t entry_keys[] = {
    DB_KEY_INIT("aaaa", 4), DB_KEY_INIT("aaab", 4), DB_KEY_INIT("aaac", 4),
    DB_KEY_INIT("aaad", 4), DB_KEY_INIT("aaae", 4), DB_KEY_INIT("aaaf", 4),
    DB_KEY_INIT("aaag", 4), DB_KEY_INIT("aaah", 4), DB_KEY_INIT("aaai", 4)
  };
#define hash_offset 5

  table.ht_table = oldtab;
  table.ht_modulus = 3;
  table.ht_count = 9;
  will_return(__wrap__hash_prime, 7);
  expect_value(__wrap__hash_prime, start, 5);
  will_return(__wrap_malloc, linktab);
  expect_value(__wrap_malloc, size, 7 * sizeof(link_head_t));
  for (i = 0; i < 7; i++) {
    will_return(__wrap_ll_init, 0);
    expect_value(__wrap_ll_init, list, &linktab[i]);
    expect_value(__wrap_ll_init, extra, &table);
  }
  for (i = 0; i < 3; i++) {
    /* Point link heads to the correct entries */
    oldtab[i].lh_first = &entries[i * 3].he_elem;

    /* Construct the testing linked list */
    for (j = 0; j < 3; j++) {
      entries[i * 3 + j].he_elem.le_object = &entries[i * 3 + j];
      entries[i * 3 + j].he_key = entry_keys[i * 3 + j];
      entries[i * 3 + j].he_elem.le_next =
	j + 1 < 3 ? &entries[i * 3 + j + 1].he_elem : 0;

      /* Set up the callbacks while we're here */
      if (i * 3 + j < 5) {
	will_return(__wrap_ll_remove, 0);
	expect_value(__wrap_ll_remove, list, &oldtab[i]);
	expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
	will_return(__wrap_ll_add, 0);
	expect_value(__wrap_ll_add, list,
		     &linktab[(i * 3 + j + hash_offset) % 7]);
	expect_value(__wrap_ll_add, new, &entries[i * 3 + j].he_elem);
	expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
	expect_value(__wrap_ll_add, elem, 0);
      } else if (i * 3 + j == 5) {
	will_return(__wrap_ll_remove, 0);
	expect_value(__wrap_ll_remove, list, &oldtab[i]);
	expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
	will_return(__wrap_ll_add, 42);
	expect_value(__wrap_ll_add, list,
		     &linktab[(i * 3 + j + hash_offset) % 7]);
	expect_value(__wrap_ll_add, new, &entries[i * 3 + j].he_elem);
	expect_value(__wrap_ll_add, loc, LINK_LOC_HEAD);
	expect_value(__wrap_ll_add, elem, 0);
      }
    }
  }
  expect_value(__wrap_free, ptr, linktab);
  expect_value(__wrap_free, ptr, oldtab);

  err = ht_resize(&table, 5);

  assert_int_equal(err, DB_ERR_UNRECOVERABLE);
  assert_int_equal(table.ht_modulus, 0);
  assert_int_equal(table.ht_count, 0);
  assert_int_equal(table.ht_rollover, 0);
  assert_int_equal(table.ht_rollunder, 0);
  assert_int_equal(table.ht_table, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerotable),
    cmocka_unit_test(test_badtable),
    cmocka_unit_test(test_frozen),
    cmocka_unit_test(test_zero_size),
    cmocka_unit_test(test_nonzero_size),
    cmocka_unit_test(test_explicit_size),
    cmocka_unit_test_setup_teardown(test_resize_zero_malloc_fails,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_no_resize_malloc_fails,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_llinit_fails,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_empty,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_rehash,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_llremove_fails,
				    malloc_setup, malloc_teardown),
    cmocka_unit_test_setup_teardown(test_lladd_fails,
				    malloc_setup, malloc_teardown)
  };

  return cmocka_run_group_tests_name("Test ht_resize.c", tests, 0, 0);
}
