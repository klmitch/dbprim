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
__wrap_le_init(link_elem_t *elem, void *object)
{
  check_expected_ptr(elem);
  check_expected_ptr(object);
  return (db_err_t)mock();
}

void
test_badargs(void **state)
{
  db_err_t err;

  err = he_init(0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_le_init_fails(void **state)
{
  db_err_t err;
  hash_entry_t entry;
  int spam;

  will_return(__wrap_le_init, 42);
  expect_value(__wrap_le_init, elem, &entry.he_elem);
  expect_value(__wrap_le_init, object, &entry);

  err = he_init(&entry, &spam);

  assert_int_equal(err, 42);
}

void
test_le_init_succeeds(void **state)
{
  db_err_t err;
  hash_entry_t entry;
  int spam;

  will_return(__wrap_le_init, 0);
  expect_value(__wrap_le_init, elem, &entry.he_elem);
  expect_value(__wrap_le_init, object, &entry);

  err = he_init(&entry, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(entry.he_magic, HASH_ENTRY_MAGIC);
  assert_ptr_equal(entry.he_table, 0);
  assert_int_equal(entry.he_hash, 0);
  assert_ptr_equal(entry.he_key.dk_key, 0);
  assert_int_equal(entry.he_key.dk_len, 0);
  assert_ptr_equal(entry.he_value, &spam);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badargs),
    cmocka_unit_test(test_le_init_fails),
    cmocka_unit_test(test_le_init_succeeds)
  };

  return cmocka_run_group_tests_name("Test he_init.c", tests, 0, 0);
}
