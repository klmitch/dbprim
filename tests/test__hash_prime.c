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
test_zero(void **state)
{
  hash_t hash;

  hash = _hash_prime(0);

  assert_int_equal(hash, 0);
}

void
test_small(void **state)
{
  hash_t hash;

  hash = _hash_prime(3);

  assert_int_equal(hash, 3);
}

void
test_large(void **state)
{
  hash_t hash;

  hash = _hash_prime(4294967292UL);

  assert_int_equal(hash, 4294967291UL);
}

void
test_find(void **state)
{
  hash_t hash;

  hash = _hash_prime(4294967280UL);

  assert_int_equal(hash, 4294967291UL);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zero),
    cmocka_unit_test(test_small),
    cmocka_unit_test(test_large),
    cmocka_unit_test(test_find)
  };

  return cmocka_run_group_tests_name("Test _hash_prime.c", tests, 0, 0);
}
