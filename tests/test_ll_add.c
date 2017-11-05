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

  err = ll_add(0, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badlist(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  list.lh_magic = 0;

  err = ll_add(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeronew(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);

  err = ll_add(&list, 0, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badnew(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  new.le_magic = 0;

  err = ll_add(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badelem(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0), elem = LINK_ELEM_INIT(0);

  elem.le_magic = 0;

  err = ll_add(&list, &new, LINK_LOC_HEAD, &elem);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_missingelem_before(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_add(&list, &new, LINK_LOC_BEFORE, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_missingelem_after(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_add(&list, &new, LINK_LOC_AFTER, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_busy(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_head_t other = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  new.le_head = &other;

  err = ll_add(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, DB_ERR_BUSY);
}

void
test_wrongtable(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_head_t other = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  elem.le_head = &other;

  err = ll_add(&list, &new, LINK_LOC_AFTER, &elem);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_unused(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  err = ll_add(&list, &new, LINK_LOC_AFTER, &elem);

  assert_int_equal(err, DB_ERR_UNUSED);
}

void
test_head_empty(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_add(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 1);
  assert_ptr_equal(new.le_head, &list);
  assert_ptr_equal(list.lh_first, &new);
  assert_ptr_equal(list.lh_last, &new);
  assert_ptr_equal(new.le_next, 0);
  assert_ptr_equal(new.le_prev, 0);
}

void
test_head_nonempty(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  list.lh_count = 1;
  list.lh_first = &elem;
  list.lh_last = &elem;
  elem.le_head = &list;

  err = ll_add(&list, &new, LINK_LOC_HEAD, 0);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 2);
  assert_ptr_equal(new.le_head, &list);
  assert_ptr_equal(list.lh_first, &new);
  assert_ptr_equal(list.lh_last, &elem);
  assert_ptr_equal(new.le_next, &elem);
  assert_ptr_equal(new.le_prev, 0);
  assert_ptr_equal(elem.le_next, 0);
  assert_ptr_equal(elem.le_prev, &new);
}

void
test_tail_empty(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);

  err = ll_add(&list, &new, LINK_LOC_TAIL, 0);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 1);
  assert_ptr_equal(new.le_head, &list);
  assert_ptr_equal(list.lh_first, &new);
  assert_ptr_equal(list.lh_last, &new);
  assert_ptr_equal(new.le_next, 0);
  assert_ptr_equal(new.le_prev, 0);
}

void
test_tail_nonempty(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elem = LINK_ELEM_INIT(0);

  list.lh_count = 1;
  list.lh_first = &elem;
  list.lh_last = &elem;
  elem.le_head = &list;

  err = ll_add(&list, &new, LINK_LOC_TAIL, 0);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 2);
  assert_ptr_equal(new.le_head, &list);
  assert_ptr_equal(list.lh_first, &elem);
  assert_ptr_equal(list.lh_last, &new);
  assert_ptr_equal(elem.le_next, &new);
  assert_ptr_equal(elem.le_prev, 0);
  assert_ptr_equal(new.le_next, 0);
  assert_ptr_equal(new.le_prev, &elem);
}

void
test_before(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elems[] = { LINK_ELEM_INIT(0), LINK_ELEM_INIT(0) };

  list.lh_count = 2;
  list.lh_first = &elems[0];
  list.lh_last = &elems[1];
  elems[0].le_next = &elems[1];
  elems[0].le_head = &list;
  elems[1].le_prev = &elems[0];
  elems[1].le_head = &list;

  err = ll_add(&list, &new, LINK_LOC_BEFORE, &elems[1]);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 3);
  assert_ptr_equal(new.le_head, &list);
  assert_ptr_equal(list.lh_first, &elems[0]);
  assert_ptr_equal(list.lh_last, &elems[1]);
  assert_ptr_equal(elems[0].le_prev, 0);
  assert_ptr_equal(elems[0].le_next, &new);
  assert_ptr_equal(new.le_prev, &elems[0]);
  assert_ptr_equal(new.le_next, &elems[1]);
  assert_ptr_equal(elems[1].le_prev, &new);
  assert_ptr_equal(elems[1].le_next, 0);
}

void
test_after(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t new = LINK_ELEM_INIT(0);
  link_elem_t elems[] = { LINK_ELEM_INIT(0), LINK_ELEM_INIT(0) };

  list.lh_count = 2;
  list.lh_first = &elems[0];
  list.lh_last = &elems[1];
  elems[0].le_next = &elems[1];
  elems[0].le_head = &list;
  elems[1].le_prev = &elems[0];
  elems[1].le_head = &list;

  err = ll_add(&list, &new, LINK_LOC_AFTER, &elems[0]);

  assert_int_equal(err, 0);
  assert_int_equal(list.lh_count, 3);
  assert_ptr_equal(new.le_head, &list);
  assert_ptr_equal(list.lh_first, &elems[0]);
  assert_ptr_equal(list.lh_last, &elems[1]);
  assert_ptr_equal(elems[0].le_prev, 0);
  assert_ptr_equal(elems[0].le_next, &new);
  assert_ptr_equal(new.le_prev, &elems[0]);
  assert_ptr_equal(new.le_next, &elems[1]);
  assert_ptr_equal(elems[1].le_prev, &new);
  assert_ptr_equal(elems[1].le_next, 0);
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
    cmocka_unit_test(test_wrongtable),
    cmocka_unit_test(test_unused),
    cmocka_unit_test(test_head_empty),
    cmocka_unit_test(test_head_nonempty),
    cmocka_unit_test(test_tail_empty),
    cmocka_unit_test(test_tail_nonempty),
    cmocka_unit_test(test_before),
    cmocka_unit_test(test_after)
  };

  return cmocka_run_group_tests_name("Test ll_add.c", tests, 0, 0);
}
