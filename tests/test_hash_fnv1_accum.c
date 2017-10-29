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
test_badstate(void **state)
{
  db_err_t err;

  err = hash_fnv1_accum(0, "data", 4);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_baddata(void **state)
{
  db_err_t err;
  hash_fnv_state_t test_state = HASH_FNV_OFFSET;

  err = hash_fnv1_accum(&test_state, 0, 4);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badlen(void **state)
{
  db_err_t err;
  hash_fnv_state_t test_state = HASH_FNV_OFFSET;

  err = hash_fnv1_accum(&test_state, "data", 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_success(void **state)
{
  db_err_t err;
  hash_fnv_state_t test_state = HASH_FNV_OFFSET;

  err = hash_fnv1_accum(&test_state, "data", 4);

  assert_int_equal(err, 0);
  assert_int_equal(test_state, 1959470013UL);

  err = hash_fnv1_accum(&test_state, "data", 4);

  assert_int_equal(err, 0);
  assert_int_equal(test_state, 3697007541UL);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badstate),
    cmocka_unit_test(test_baddata),
    cmocka_unit_test(test_badlen),
    cmocka_unit_test(test_success)
  };

  return cmocka_run_group_tests_name("Test hash_fnv1_accum.c", tests, 0, 0);
}
