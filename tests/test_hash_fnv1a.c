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
__wrap_hash_fnv_init(hash_fnv_state_t *state)
{
  check_expected_ptr(state);
  return (db_err_t)mock();
}

db_err_t
__wrap_hash_fnv1a_accum(hash_fnv_state_t *state, void *data, size_t len)
{
  check_expected_ptr(state);
  check_expected_ptr(data);
  check_expected(len);
  return (db_err_t)mock();
}

hash_t
__wrap_hash_fnv_final(hash_fnv_state_t *state)
{
  check_expected_ptr(state);
  return (hash_t)mock();
}

void
test_badkey(void **state)
{
  hash_t hash;

  hash = hash_fnv1a(0, 0);

  assert_int_equal(hash, 0);
}

void
test_badlen(void **state)
{
  hash_t hash;
  db_key_t key = DB_KEY_INIT("data", 0);

  hash = hash_fnv1a(0, &key);

  assert_int_equal(hash, 0);
}

void
test_badvalue(void **state)
{
  hash_t hash;
  db_key_t key = DB_KEY_INIT(0, 4);

  hash = hash_fnv1a(0, &key);

  assert_int_equal(hash, 0);
}

void
test_success(void **state)
{
  hash_t hash;
  db_key_t key = DB_KEY_INIT("data", 4);

  will_return(__wrap_hash_fnv_init, 0);
  expect_any(__wrap_hash_fnv_init, state);
  will_return(__wrap_hash_fnv1a_accum, 0);
  expect_any(__wrap_hash_fnv1a_accum, state);
  expect_value(__wrap_hash_fnv1a_accum, data, key.dk_key);
  expect_value(__wrap_hash_fnv1a_accum, len, 4);
  will_return(__wrap_hash_fnv_final, 42);
  expect_any(__wrap_hash_fnv_final, state);

  hash = hash_fnv1a(0, &key);

  assert_int_equal(hash, 42);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badkey),
    cmocka_unit_test(test_badlen),
    cmocka_unit_test(test_badvalue),
    cmocka_unit_test(test_success)
  };

  return cmocka_run_group_tests_name("Test hash_fnv1a.c", tests, 0, 0);
}
