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

void
test_zerolist(void **state)
{
  db_err_t err;
  link_elem_t elem = LINK_ELEM_INIT(0);

  err = ll_remove(0, &elem);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badlist(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  list.lh_magic = 0;

  err = ll_remove(&list, &elem);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeroelem(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);

  err = ll_remove(&list, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badelem(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  elem.le_magic = 0;

  err = ll_remove(&list, &elem);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_unused(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  err = ll_remove(&list, &elem);

  assert_int_equal(err, DB_ERR_UNUSED);
}

void
test_wrongtable(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_head_t other = LINK_HEAD_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  elem.le_head = &other;

  err = ll_remove(&list, &elem);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_remove_head(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  list.lh_count = 3;
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_remove(&list, &elems[0]);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 2);
  assert_ptr_equal(list.lh_first, &elems[1]);
  assert_ptr_equal(list.lh_last, &elems[2]);
  assert_ptr_equal(elems[0].le_head, 0);
  assert_ptr_equal(elems[0].le_prev, 0);
  assert_ptr_equal(elems[0].le_next, 0);
  assert_ptr_equal(elems[1].le_head, &list);
  assert_ptr_equal(elems[1].le_prev, 0);
  assert_ptr_equal(elems[1].le_next, &elems[2]);
  assert_ptr_equal(elems[2].le_head, &list);
  assert_ptr_equal(elems[2].le_prev, &elems[1]);
  assert_ptr_equal(elems[2].le_next, 0);
}

void
test_remove_middle(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  list.lh_count = 3;
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_remove(&list, &elems[1]);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 2);
  assert_ptr_equal(list.lh_first, &elems[0]);
  assert_ptr_equal(list.lh_last, &elems[2]);
  assert_ptr_equal(elems[0].le_head, &list);
  assert_ptr_equal(elems[0].le_prev, 0);
  assert_ptr_equal(elems[0].le_next, &elems[2]);
  assert_ptr_equal(elems[1].le_head, 0);
  assert_ptr_equal(elems[1].le_prev, 0);
  assert_ptr_equal(elems[1].le_next, 0);
  assert_ptr_equal(elems[2].le_head, &list);
  assert_ptr_equal(elems[2].le_prev, &elems[0]);
  assert_ptr_equal(elems[2].le_next, 0);
}

void
test_remove_tail(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  list.lh_count = 3;
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_remove(&list, &elems[2]);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 2);
  assert_ptr_equal(list.lh_first, &elems[0]);
  assert_ptr_equal(list.lh_last, &elems[1]);
  assert_ptr_equal(elems[0].le_head, &list);
  assert_ptr_equal(elems[0].le_prev, 0);
  assert_ptr_equal(elems[0].le_next, &elems[1]);
  assert_ptr_equal(elems[1].le_head, &list);
  assert_ptr_equal(elems[1].le_prev, &elems[0]);
  assert_ptr_equal(elems[1].le_next, 0);
  assert_ptr_equal(elems[2].le_head, 0);
  assert_ptr_equal(elems[2].le_prev, 0);
  assert_ptr_equal(elems[2].le_next, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerolist),
    cmocka_unit_test(test_badlist),
    cmocka_unit_test(test_zeroelem),
    cmocka_unit_test(test_badelem),
    cmocka_unit_test(test_unused),
    cmocka_unit_test(test_wrongtable),
    cmocka_unit_test(test_remove_head),
    cmocka_unit_test(test_remove_middle),
    cmocka_unit_test(test_remove_tail)
  };

  return cmocka_run_group_tests_name("Test ll_remove.c", tests, 0, 0);
}
