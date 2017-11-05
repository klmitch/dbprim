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
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_move(0, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badlist(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  list.lh_magic = 0;

  err = ll_move(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeronew(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);

  err = ll_move(&list, 0, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badnew(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  new.le_magic = 0;

  err = ll_move(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badelem(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0), elem = LINK_ELEM_INIT(0);

  elem.le_magic = 0;

  err = ll_move(&list, &new, LINK_LOC_HEAD, &elem);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_missingelem_before(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_move(&list, &new, LINK_LOC_BEFORE, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_missingelem_after(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_move(&list, &new, LINK_LOC_AFTER, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_busy(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_move(&list, &new, LINK_LOC_AFTER, &new);

  assert_int_equal(err, DB_ERR_BUSY);
}

void
test_new_unused(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_move(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_UNUSED);
}

void
test_new_wrongtable(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_head_t other = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  new.le_head = &other;

  err = ll_move(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_elem_unused(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  new.le_head = &list;

  err = ll_move(&list, &new, LINK_LOC_AFTER, &elem);

  assert_int_equal(err, DB_ERR_UNUSED);
}

void
test_elem_wrongtable(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_head_t other = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  new.le_head = &list;
  elem.le_head = &other;

  err = ll_move(&list, &new, LINK_LOC_AFTER, &elem);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_1elem_head(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  list.lh_first = &new;
  list.lh_last = &new;
  new.le_head = &list;

  err = ll_move(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &new);
  assert_ptr_equal(list.lh_last, &new);
  assert_ptr_equal(new.le_prev, 0);
  assert_ptr_equal(new.le_next, 0);
}

void
test_1elem_tail(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  list.lh_first = &new;
  list.lh_last = &new;
  new.le_head = &list;

  err = ll_move(&list, &new, LINK_LOC_TAIL, 0);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &new);
  assert_ptr_equal(list.lh_last, &new);
  assert_ptr_equal(new.le_prev, 0);
  assert_ptr_equal(new.le_next, 0);
}

void
test_move2head(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_move(&list, &elems[1], LINK_LOC_HEAD, 0);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &elems[1]);
  assert_ptr_equal(list.lh_last, &elems[2]);
  assert_ptr_equal(elems[0].le_prev, &elems[1]);
  assert_ptr_equal(elems[0].le_next, &elems[2]);
  assert_ptr_equal(elems[1].le_prev, 0);
  assert_ptr_equal(elems[1].le_next, &elems[0]);
  assert_ptr_equal(elems[2].le_prev, &elems[0]);
  assert_ptr_equal(elems[2].le_next, 0);
}

void
test_move2tail(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_move(&list, &elems[1], LINK_LOC_TAIL, 0);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &elems[0]);
  assert_ptr_equal(list.lh_last, &elems[1]);
  assert_ptr_equal(elems[0].le_prev, 0);
  assert_ptr_equal(elems[0].le_next, &elems[2]);
  assert_ptr_equal(elems[1].le_prev, &elems[2]);
  assert_ptr_equal(elems[1].le_next, 0);
  assert_ptr_equal(elems[2].le_prev, &elems[0]);
  assert_ptr_equal(elems[2].le_next, &elems[1]);
}

void
test_move_tail2head(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_move(&list, &elems[2], LINK_LOC_HEAD, 0);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &elems[2]);
  assert_ptr_equal(list.lh_last, &elems[1]);
  assert_ptr_equal(elems[0].le_prev, &elems[2]);
  assert_ptr_equal(elems[0].le_next, &elems[1]);
  assert_ptr_equal(elems[1].le_prev, &elems[0]);
  assert_ptr_equal(elems[1].le_next, 0);
  assert_ptr_equal(elems[2].le_prev, 0);
  assert_ptr_equal(elems[2].le_next, &elems[0]);
}

void
test_move_head2tail(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_move(&list, &elems[0], LINK_LOC_TAIL, 0);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &elems[1]);
  assert_ptr_equal(list.lh_last, &elems[0]);
  assert_ptr_equal(elems[0].le_prev, &elems[2]);
  assert_ptr_equal(elems[0].le_next, 0);
  assert_ptr_equal(elems[1].le_prev, 0);
  assert_ptr_equal(elems[1].le_next, &elems[2]);
  assert_ptr_equal(elems[2].le_prev, &elems[1]);
  assert_ptr_equal(elems[2].le_next, &elems[0]);
}

void
test_move_head_before_tail(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_move(&list, &elems[0], LINK_LOC_BEFORE, &elems[2]);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &elems[1]);
  assert_ptr_equal(list.lh_last, &elems[2]);
  assert_ptr_equal(elems[0].le_prev, &elems[1]);
  assert_ptr_equal(elems[0].le_next, &elems[2]);
  assert_ptr_equal(elems[1].le_prev, 0);
  assert_ptr_equal(elems[1].le_next, &elems[0]);
  assert_ptr_equal(elems[2].le_prev, &elems[0]);
  assert_ptr_equal(elems[2].le_next, 0);
}

void
test_move_tail_after_head(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };

  list.lh_first = &elems[0];
  list.lh_last = &elems[2];
  for (i = 0; i < 3; i++) {
    elems[i].le_head = &list;
    elems[i].le_prev = i ? &elems[i - 1] : 0;
    elems[i].le_next = i < 2 ? &elems[i + 1] : 0;
  }

  err = ll_move(&list, &elems[2], LINK_LOC_AFTER, &elems[0]);

  assert_int_equal(err, 0);
  assert_ptr_equal(list.lh_first, &elems[0]);
  assert_ptr_equal(list.lh_last, &elems[1]);
  assert_ptr_equal(elems[0].le_prev, 0);
  assert_ptr_equal(elems[0].le_next, &elems[2]);
  assert_ptr_equal(elems[1].le_prev, &elems[2]);
  assert_ptr_equal(elems[1].le_next, 0);
  assert_ptr_equal(elems[2].le_prev, &elems[0]);
  assert_ptr_equal(elems[2].le_next, &elems[1]);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerolist),
    cmocka_unit_test(test_badlist),
    cmocka_unit_test(test_zeronew),
    cmocka_unit_test(test_badnew),
    cmocka_unit_test(test_badelem),
    cmocka_unit_test(test_missingelem_before),
    cmocka_unit_test(test_missingelem_after),
    cmocka_unit_test(test_busy),
    cmocka_unit_test(test_new_unused),
    cmocka_unit_test(test_new_wrongtable),
    cmocka_unit_test(test_elem_unused),
    cmocka_unit_test(test_elem_wrongtable),
    cmocka_unit_test(test_1elem_head),
    cmocka_unit_test(test_1elem_tail),
    cmocka_unit_test(test_move2head),
    cmocka_unit_test(test_move2tail),
    cmocka_unit_test(test_move_tail2head),
    cmocka_unit_test(test_move_head2tail),
    cmocka_unit_test(test_move_head_before_tail),
    cmocka_unit_test(test_move_tail_after_head)
  };

  return cmocka_run_group_tests_name("Test ll_move.c", tests, 0, 0);
}
