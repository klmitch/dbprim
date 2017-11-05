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
__wrap_ll_remove(link_head_t *list, link_elem_t *elem)
{
  check_expected_ptr(list);
  check_expected_ptr(elem);

  assert_ptr_equal(list->lh_first, elem);

  list->lh_first = elem->le_next;

  return (db_err_t)mock();
}

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

  err = ll_flush(0, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badlist(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);

  list.lh_magic = 0;

  err = ll_flush(&list, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_iterfails(void **state)
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
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    if (i < 4) {
      will_return(__wrap_ll_remove, 0);
      expect_value(__wrap_ll_remove, list, &list);
      expect_value(__wrap_ll_remove, elem, &elems[i]);
      will_return(test_iter, i != 3 ? 0 : 42);
      expect_value(test_iter, list, &list);
      expect_value(test_iter, elem, &elems[i]);
      expect_value(test_iter, extra, &spam);
    }
  }

  err = ll_flush(&list, test_iter, &spam);

  assert_int_equal(err, 42);
  assert_int_equal(list.lh_count, 5); /* remember, wrapper doesn't decrement */
  assert_ptr_equal(list.lh_first, &elems[4]);
  assert_ptr_equal(list.lh_last, &elems[4]);
}

void
test_itersucceeds(void **state)
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
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    will_return(__wrap_ll_remove, 0);
    expect_value(__wrap_ll_remove, list, &list);
    expect_value(__wrap_ll_remove, elem, &elems[i]);
    will_return(test_iter, 0);
    expect_value(test_iter, list, &list);
    expect_value(test_iter, elem, &elems[i]);
    expect_value(test_iter, extra, &spam);
  }

  err = ll_flush(&list, test_iter, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 0);
  assert_ptr_equal(list.lh_first, 0);
  assert_ptr_equal(list.lh_last, 0);
}

void
test_noiter(void **state)
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
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    will_return(__wrap_ll_remove, 0);
    expect_value(__wrap_ll_remove, list, &list);
    expect_value(__wrap_ll_remove, elem, &elems[i]);
  }

  err = ll_flush(&list, 0, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 0);
  assert_ptr_equal(list.lh_first, 0);
  assert_ptr_equal(list.lh_last, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerolist),
    cmocka_unit_test(test_badlist),
    cmocka_unit_test(test_iterfails),
    cmocka_unit_test(test_itersucceeds),
    cmocka_unit_test(test_noiter)
  };

  return cmocka_run_group_tests_name("Test ll_flush.c", tests, 0, 0);
}
