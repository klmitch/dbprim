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

#include "linklist_int.h"

db_err_t
test_iter(link_head_t *list, link_elem_t *elem, void *extra)
{
  check_expected_ptr(list);
  check_expected_ptr(elem);
  check_expected_ptr(extra);
  return (db_err_t)mock();
}

void
test_zerolist(void **state)
{
  db_err_t err;

  err = ll_iter(0, 0, test_iter, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badlist(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);

  list.lh_magic = 0;

  err = ll_iter(&list, 0, test_iter, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badstart(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t start = LINK_ELEM_INIT(0);

  start.le_magic = 0;

  err = ll_iter(&list, &start, test_iter, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeroiterfunc(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);

  err = ll_iter(&list, 0, 0, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_wrongtable(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_head_t other = LINK_HEAD_INIT(0);
  link_elem_t start = LINK_ELEM_INIT(0);

  start.le_head = &other;

  err = ll_iter(&list, &start, test_iter, 0, 0);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_unused(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t start = LINK_ELEM_INIT(0);

  err = ll_iter(&list, &start, test_iter, 0, 0);

  assert_int_equal(err, DB_ERR_UNUSED);
}

void
test_forward(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int spam;

  list.lh_count = 5;
  list.lh_first = &elems[0];
  list.lh_last = &elems[4];
  for (i = 0; i < 5; i++) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_prev = i ? &elems[i - 1] : 0;

    will_return(test_iter, 0);
    expect_value(test_iter, list, &list);
    expect_value(test_iter, elem, &elems[i]);
    expect_value(test_iter, extra, &spam);
  }

  err = ll_iter(&list, 0, test_iter, &spam, 0);

  assert_int_equal(err, 0);
}

void
test_forward_abort(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int spam;

  list.lh_count = 5;
  list.lh_first = &elems[0];
  list.lh_last = &elems[4];
  for (i = 0; i < 5; i++) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_prev = i ? &elems[i - 1] : 0;

    if (i < 4) {
      will_return(test_iter, i == 3 ? 42 : 0);
      expect_value(test_iter, list, &list);
      expect_value(test_iter, elem, &elems[i]);
      expect_value(test_iter, extra, &spam);
    }
  }

  err = ll_iter(&list, 0, test_iter, &spam, 0);

  assert_int_equal(err, 42);
}

void
test_forward_start(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int spam;

  list.lh_count = 5;
  list.lh_first = &elems[0];
  list.lh_last = &elems[4];
  for (i = 0; i < 5; i++) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_prev = i ? &elems[i - 1] : 0;

    if (i > 2) {
      will_return(test_iter, 0);
      expect_value(test_iter, list, &list);
      expect_value(test_iter, elem, &elems[i]);
      expect_value(test_iter, extra, &spam);
    }
  }

  err = ll_iter(&list, &elems[3], test_iter, &spam, 0);

  assert_int_equal(err, 0);
}

void
test_reverse(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int spam;

  list.lh_count = 5;
  list.lh_first = &elems[0];
  list.lh_last = &elems[4];
  for (i = 4; i >= 0; i--) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_prev = i ? &elems[i - 1] : 0;

    will_return(test_iter, 0);
    expect_value(test_iter, list, &list);
    expect_value(test_iter, elem, &elems[i]);
    expect_value(test_iter, extra, &spam);
  }

  err = ll_iter(&list, 0, test_iter, &spam, DB_FLAG_REVERSE);

  assert_int_equal(err, 0);
}

void
test_reverse_abort(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int spam;

  list.lh_count = 5;
  list.lh_first = &elems[0];
  list.lh_last = &elems[4];
  for (i = 4; i >= 0; i--) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_prev = i ? &elems[i - 1] : 0;

    if (i > 2) {
      will_return(test_iter, i == 3 ? 42 : 0);
      expect_value(test_iter, list, &list);
      expect_value(test_iter, elem, &elems[i]);
      expect_value(test_iter, extra, &spam);
    }
  }

  err = ll_iter(&list, 0, test_iter, &spam, DB_FLAG_REVERSE);

  assert_int_equal(err, 42);
}

void
test_reverse_start(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int spam;

  list.lh_count = 5;
  list.lh_first = &elems[0];
  list.lh_last = &elems[4];
  for (i = 4; i >= 0; i--) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_prev = i ? &elems[i - 1] : 0;

    if (i < 4) {
      will_return(test_iter, 0);
      expect_value(test_iter, list, &list);
      expect_value(test_iter, elem, &elems[i]);
      expect_value(test_iter, extra, &spam);
    }
  }

  err = ll_iter(&list, &elems[3], test_iter, &spam, DB_FLAG_REVERSE);

  assert_int_equal(err, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerolist),
    cmocka_unit_test(test_badlist),
    cmocka_unit_test(test_badstart),
    cmocka_unit_test(test_zeroiterfunc),
    cmocka_unit_test(test_wrongtable),
    cmocka_unit_test(test_unused),
    cmocka_unit_test(test_forward),
    cmocka_unit_test(test_forward_abort),
    cmocka_unit_test(test_forward_start),
    cmocka_unit_test(test_reverse),
    cmocka_unit_test(test_reverse_abort),
    cmocka_unit_test(test_reverse_start)
  };

  return cmocka_run_group_tests_name("Test ll_iter.c", tests, 0, 0);
}
