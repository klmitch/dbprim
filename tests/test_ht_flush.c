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

  assert_ptr_equal(list->lh_first, elem);

  list->lh_first = elem->le_next;

  return (db_err_t)mock();
}

db_err_t
__wrap_ht_free(hash_table_t *table)
{
  check_expected_ptr(table);
  return (db_err_t)mock();
}

db_err_t
test_iter(hash_table_t *table, hash_entry_t *ent, void *extra)
{
  check_expected_ptr(table);
  check_expected_ptr(ent);
  check_expected_ptr(extra);

  assert_true(table->ht_flags & HASH_FLAG_FREEZE);
  assert_ptr_equal(ent->he_table, 0);

  return (db_err_t)mock();
}

void
test_zerotable(void **state)
{
  db_err_t err;

  err = ht_flush(0, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtable(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  table.ht_magic = 0;

  err = ht_flush(&table, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_frozen(void **state)
{
  db_err_t err;
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);

  table.ht_flags |= HASH_FLAG_FREEZE;

  err = ht_flush(&table, 0, 0);

  assert_int_equal(err, DB_ERR_FROZEN);
}

void
test_callback(void **state)
{
  int i, j;
  db_err_t err;
  link_head_t linktab[] = {
    LINK_HEAD_INIT(0), LINK_HEAD_INIT(0), LINK_HEAD_INIT(0)
  };
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entries[] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0)
  };
  int spam;

  table.ht_table = linktab;
  table.ht_modulus = 3;
  table.ht_count = 9;
  for (i = 0; i < 3; i++) {
    /* Point link heads to the correct entries */
    linktab[i].lh_first = &entries[i * 3].he_elem;

    /* Construct the testing linked list */
    for (j = 0; j < 3; j++) {
      entries[i * 3 + j].he_elem.le_object = &entries[i * 3 + j];
      entries[i * 3 + j].he_elem.le_next =
	j + 1 < 3 ? &entries[i * 3 + j + 1].he_elem : 0;

      /* Set up the callbacks while we're here */
      will_return(__wrap_ll_remove, 0);
      expect_value(__wrap_ll_remove, list, &table.ht_table[i]);
      expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
      will_return(test_iter, 0);
      expect_value(test_iter, table, &table);
      expect_value(test_iter, ent, &entries[i * 3 + j]);
      expect_value(test_iter, extra, &spam);
    }
  }

  err = ht_flush(&table, test_iter, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_count, 0);
  assert_ptr_equal(linktab[0].lh_first, 0);
  assert_ptr_equal(linktab[1].lh_first, 0);
  assert_ptr_equal(linktab[2].lh_first, 0);
}

void
test_callback_abort(void **state)
{
  int i, j;
  db_err_t err;
  link_head_t linktab[] = {
    LINK_HEAD_INIT(0), LINK_HEAD_INIT(0), LINK_HEAD_INIT(0)
  };
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entries[] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0)
  };
  int spam;

  table.ht_table = linktab;
  table.ht_modulus = 3;
  table.ht_count = 9;
  for (i = 0; i < 3; i++) {
    /* Point link heads to the correct entries */
    linktab[i].lh_first = &entries[i * 3].he_elem;

    /* Construct the testing linked list */
    for (j = 0; j < 3; j++) {
      entries[i * 3 + j].he_elem.le_object = &entries[i * 3 + j];
      entries[i * 3 + j].he_elem.le_next =
	j + 1 < 3 ? &entries[i * 3 + j + 1].he_elem : 0;

      /* Set up the callbacks while we're here */
      if (i * 3 + j < 5) {
	will_return(__wrap_ll_remove, 0);
	expect_value(__wrap_ll_remove, list, &table.ht_table[i]);
	expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
	will_return(test_iter, (i * 3 + j == 4) ? 42 : 0);
	expect_value(test_iter, table, &table);
	expect_value(test_iter, ent, &entries[i * 3 + j]);
	expect_value(test_iter, extra, &spam);
      }
    }
  }

  err = ht_flush(&table, test_iter, &spam);

  assert_int_equal(err, 42);
  assert_int_equal(table.ht_count, 4);
  assert_ptr_equal(linktab[0].lh_first, 0);
  assert_ptr_equal(linktab[1].lh_first, &entries[5].he_elem);
  assert_ptr_equal(linktab[2].lh_first, &entries[6].he_elem);
}

void
test_nocallback(void **state)
{
  int i, j;
  db_err_t err;
  link_head_t linktab[] = {
    LINK_HEAD_INIT(0), LINK_HEAD_INIT(0), LINK_HEAD_INIT(0)
  };
  hash_table_t table = HASH_TABLE_INIT(0, 0, 0, 0, 0);
  hash_entry_t entries[] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0)
  };

  table.ht_table = linktab;
  table.ht_modulus = 3;
  table.ht_count = 9;
  for (i = 0; i < 3; i++) {
    /* Point link heads to the correct entries */
    linktab[i].lh_first = &entries[i * 3].he_elem;

    /* Construct the testing linked list */
    for (j = 0; j < 3; j++) {
      entries[i * 3 + j].he_elem.le_object = &entries[i * 3 + j];
      entries[i * 3 + j].he_elem.le_next =
	j + 1 < 3 ? &entries[i * 3 + j + 1].he_elem : 0;

      /* Set up the callbacks while we're here */
      will_return(__wrap_ll_remove, 0);
      expect_value(__wrap_ll_remove, list, &table.ht_table[i]);
      expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
    }
  }

  err = ht_flush(&table, 0, 0);

  assert_int_equal(err, 0);
  assert_int_equal(table.ht_count, 0);
  assert_ptr_equal(linktab[0].lh_first, 0);
  assert_ptr_equal(linktab[1].lh_first, 0);
  assert_ptr_equal(linktab[2].lh_first, 0);
}

void
test_autoshrink(void **state)
{
  int i, j;
  db_err_t err;
  link_head_t linktab[] = {
    LINK_HEAD_INIT(0), LINK_HEAD_INIT(0), LINK_HEAD_INIT(0)
  };
  hash_table_t table = HASH_TABLE_INIT(HASH_FLAG_AUTOSHRINK, 0, 0, 0, 0);
  hash_entry_t entries[] = {
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0),
    HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0), HASH_ENTRY_INIT(0)
  };

  table.ht_table = linktab;
  table.ht_modulus = 3;
  table.ht_count = 9;
  for (i = 0; i < 3; i++) {
    /* Point link heads to the correct entries */
    linktab[i].lh_first = &entries[i * 3].he_elem;

    /* Construct the testing linked list */
    for (j = 0; j < 3; j++) {
      entries[i * 3 + j].he_elem.le_object = &entries[i * 3 + j];
      entries[i * 3 + j].he_elem.le_next =
	j + 1 < 3 ? &entries[i * 3 + j + 1].he_elem : 0;

      /* Set up the callbacks while we're here */
      will_return(__wrap_ll_remove, 0);
      expect_value(__wrap_ll_remove, list, &table.ht_table[i]);
      expect_value(__wrap_ll_remove, elem, &entries[i * 3 + j].he_elem);
    }
  }
  will_return(__wrap_ht_free, 42);
  expect_value(__wrap_ht_free, table, &table);

  err = ht_flush(&table, 0, 0);

  assert_int_equal(err, 42);
  assert_int_equal(table.ht_count, 0);
  assert_ptr_equal(linktab[0].lh_first, 0);
  assert_ptr_equal(linktab[1].lh_first, 0);
  assert_ptr_equal(linktab[2].lh_first, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerotable),
    cmocka_unit_test(test_badtable),
    cmocka_unit_test(test_frozen),
    cmocka_unit_test(test_callback),
    cmocka_unit_test(test_callback_abort),
    cmocka_unit_test(test_nocallback),
    cmocka_unit_test(test_autoshrink)
  };

  return cmocka_run_group_tests_name("Test ht_flush.c", tests, 0, 0);
}
