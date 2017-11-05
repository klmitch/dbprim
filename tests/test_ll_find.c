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

int
link_comp(db_key_t *key, void *obj)
{
  check_expected_ptr(key);
  check_expected_ptr(obj);
  return (int)mock();
}

void
test_zerolist(void **state)
{
  db_err_t err;
  link_elem_t *elem;
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ll_find(0, &elem, link_comp, 0, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badlist(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t *elem;
  db_key_t key = DB_KEY_INIT("spam", 4);

  list.lh_magic = 0;

  err = ll_find(&list, &elem, link_comp, 0, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeroelemp(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ll_find(&list, 0, link_comp, 0, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zerocompfunc(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t *elem;
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = ll_find(&list, &elem, 0, 0, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zerokey(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t *elem;

  err = ll_find(&list, &elem, link_comp, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badstart(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t start = LINK_ELEM_INIT(0);
  link_elem_t *elem;
  db_key_t key = DB_KEY_INIT("spam", 4);

  start.le_magic = 0;

  err = ll_find(&list, &elem, link_comp, &start, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_wrongtable(void **state)
{
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_head_t other = LINK_HEAD_INIT(0);
  link_elem_t start = LINK_ELEM_INIT(0);
  link_elem_t *elem;
  db_key_t key = DB_KEY_INIT("spam", 4);

  start.le_head = &other;

  err = ll_find(&list, &elem, link_comp, &start, &key);

  assert_int_equal(err, DB_ERR_WRONGTABLE);
}

void
test_noentry(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t sentinel = LINK_ELEM_INIT(0);
  link_elem_t *elem = &sentinel;
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int objs[5];
  db_key_t key = DB_KEY_INIT("spam", 4);

  list.lh_first = &elems[0];
  for (i = 0; i < 5; i++) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_object = &objs[i];
    will_return(link_comp, 1);
    expect_value(link_comp, key, &key);
    expect_value(link_comp, obj, &objs[i]);
  }

  err = ll_find(&list, &elem, link_comp, 0, &key);

  assert_int_equal(err, DB_ERR_NOENTRY);
  assert_ptr_equal(elem, &sentinel);
}

void
test_found(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t sentinel = LINK_ELEM_INIT(0);
  link_elem_t *elem = &sentinel;
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int objs[5];
  db_key_t key = DB_KEY_INIT("spam", 4);

  list.lh_first = &elems[0];
  for (i = 0; i < 5; i++) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_object = &objs[i];
    if (i < 4) {
      will_return(link_comp, i != 3);
      expect_value(link_comp, key, &key);
      expect_value(link_comp, obj, &objs[i]);
    }
  }

  err = ll_find(&list, &elem, link_comp, 0, &key);

  assert_int_equal(err, 0);
  assert_ptr_equal(elem, &elems[3]);
}

void
test_partial(void **state)
{
  int i;
  db_err_t err;
  link_head_t list = LINK_HEAD_INIT(0);
  link_elem_t sentinel = LINK_ELEM_INIT(0);
  link_elem_t *elem = &sentinel;
  link_elem_t elems[] = {
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0), LINK_ELEM_INIT(0),
    LINK_ELEM_INIT(0), LINK_ELEM_INIT(0)
  };
  int objs[5];
  db_key_t key = DB_KEY_INIT("spam", 4);

  list.lh_first = &elems[0];
  for (i = 0; i < 5; i++) {
    elems[i].le_head = &list;
    elems[i].le_next = i < 4 ? &elems[i + 1] : 0;
    elems[i].le_object = &objs[i];
    if (i > 2) {
      will_return(link_comp, 1);
      expect_value(link_comp, key, &key);
      expect_value(link_comp, obj, &objs[i]);
    }
  }

  err = ll_find(&list, &elem, link_comp, &elems[3], &key);

  assert_int_equal(err, DB_ERR_NOENTRY);
  assert_ptr_equal(elem, &sentinel);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerolist),
    cmocka_unit_test(test_badlist),
    cmocka_unit_test(test_zeroelemp),
    cmocka_unit_test(test_zerocompfunc),
    cmocka_unit_test(test_zerokey),
    cmocka_unit_test(test_badstart),
    cmocka_unit_test(test_wrongtable),
    cmocka_unit_test(test_noentry),
    cmocka_unit_test(test_found),
    cmocka_unit_test(test_partial)
  };

  return cmocka_run_group_tests_name("Test ll_find.c", tests, 0, 0);
}
