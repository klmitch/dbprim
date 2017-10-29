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

db_err_t
__wrap_ll_init(link_head_t *list, void *extra)
{
  check_expected_ptr(list);
  check_expected_ptr(extra);
  return (db_err_t)mock();
}

void
test_badhead(void **state)
{
  db_err_t err;

  err = sh_init(0, SMAT_LOC_FIRST, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badelem(void **state)
{
  db_err_t err;
  smat_head_t head;

  err = sh_init(&head, 1234, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_ll_init_fails(void **state)
{
  db_err_t err;
  smat_head_t head;
  int spam;

  will_return(__wrap_ll_init, 42);
  expect_value(__wrap_ll_init, list, &head.sh_head);
  expect_value(__wrap_ll_init, extra, &spam);

  err = sh_init(&head, SMAT_LOC_FIRST, &spam);

  assert_int_equal(err, 42);
}

void
test_first(void **state)
{
  db_err_t err;
  smat_head_t head;
  int spam;

  will_return(__wrap_ll_init, 0);
  expect_value(__wrap_ll_init, list, &head.sh_head);
  expect_value(__wrap_ll_init, extra, &spam);

  err = sh_init(&head, SMAT_LOC_FIRST, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(head.sh_magic, SMAT_HEAD_MAGIC);
  assert_int_equal(head.sh_elem, SMAT_LOC_FIRST);
  assert_ptr_equal(head.sh_table, 0);
}

void
test_second(void **state)
{
  db_err_t err;
  smat_head_t head;
  int spam;

  will_return(__wrap_ll_init, 0);
  expect_value(__wrap_ll_init, list, &head.sh_head);
  expect_value(__wrap_ll_init, extra, &spam);

  err = sh_init(&head, SMAT_LOC_SECOND, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(head.sh_magic, SMAT_HEAD_MAGIC);
  assert_int_equal(head.sh_elem, SMAT_LOC_SECOND);
  assert_ptr_equal(head.sh_table, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badhead),
    cmocka_unit_test(test_badelem),
    cmocka_unit_test(test_ll_init_fails),
    cmocka_unit_test(test_first),
    cmocka_unit_test(test_second)
  };

  return cmocka_run_group_tests_name("Test sh_init.c", tests, 0, 0);
}
