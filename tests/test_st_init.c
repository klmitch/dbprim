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

#include "sparsemat_int.h"

int
__wrap_hash_comp(hash_table_t *table, db_key_t *key1, db_key_t *key2)
{
  check_expected_ptr(table);
  check_expected_ptr(key1);
  check_expected_ptr(key2);
  return (int)mock();
}

hash_t
__wrap_hash_fnv1a(hash_table_t *table, db_key_t *key)
{
  check_expected_ptr(table);
  check_expected_ptr(key);
  return (hash_t)mock();
}

db_err_t
__wrap__smat_resize(hash_table_t *table, hash_t new_mod)
{
  check_expected_ptr(table);
  check_expected(new_mod);
  return (db_err_t)mock();
}

db_err_t
__wrap_ht_init(hash_table_t *table, db_flag_t flags, hash_func_t func,
	       hash_comp_t comp, hash_resize_t resize, void *extra,
	       hash_t init_mod)
{
  check_expected_ptr(table);
  check_expected(flags);
  check_expected(func);
  check_expected(comp);
  check_expected(resize);
  check_expected_ptr(extra);
  check_expected(init_mod);
  return (db_err_t)mock();
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
  };

  return cmocka_run_group_tests_name("Test st_init.c", tests, 0, 0);
}
