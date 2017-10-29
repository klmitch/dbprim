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
test_badkey1(void **state)
{
  int result;
  db_key_t key2 = DB_KEY_INIT("spam", 4);

  result = hash_comp(0, 0, &key2);

  assert_int_equal(result, 1);
}

void
test_badlen1(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT("spam", 0);
  db_key_t key2 = DB_KEY_INIT("spam", 4);

  result = hash_comp(0, &key1, &key2);

  assert_int_equal(result, 1);
}

void
test_badvalue1(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT(0, 4);
  db_key_t key2 = DB_KEY_INIT("spam", 4);

  result = hash_comp(0, &key1, &key2);

  assert_int_equal(result, 1);
}

void
test_badkey2(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT("spam", 4);

  result = hash_comp(0, &key1, 0);

  assert_int_equal(result, 1);
}

void
test_badlen2(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT("spam", 4);
  db_key_t key2 = DB_KEY_INIT("spam", 0);

  result = hash_comp(0, &key1, &key2);

  assert_int_equal(result, 1);
}

void
test_badvalue2(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT("spam", 4);
  db_key_t key2 = DB_KEY_INIT(0, 4);

  result = hash_comp(0, &key1, &key2);

  assert_int_equal(result, 1);
}

void
test_match(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT("spam", 4);
  db_key_t key2 = DB_KEY_INIT("spam", 4);

  result = hash_comp(0, &key1, &key2);

  assert_int_equal(result, 0);
}

void
test_nomatch_len(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT("spam", 4);
  db_key_t key2 = DB_KEY_INIT("spam", 6);

  result = hash_comp(0, &key1, &key2);

  assert_int_not_equal(result, 0);
}

void
test_nomatch_value(void **state)
{
  int result;
  db_key_t key1 = DB_KEY_INIT("spam", 4);
  db_key_t key2 = DB_KEY_INIT("test", 4);

  result = hash_comp(0, &key1, &key2);

  assert_int_not_equal(result, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badkey1),
    cmocka_unit_test(test_badlen1),
    cmocka_unit_test(test_badvalue1),
    cmocka_unit_test(test_badkey2),
    cmocka_unit_test(test_badlen2),
    cmocka_unit_test(test_badvalue2),
    cmocka_unit_test(test_match),
    cmocka_unit_test(test_nomatch_len),
    cmocka_unit_test(test_nomatch_value)
  };

  return cmocka_run_group_tests_name("Test hash_comp.c", tests, 0, 0);
}
