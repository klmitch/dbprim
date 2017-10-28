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
test_badargs(void **state)
{
  db_err_t err;

  err = le_init(0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_goodargs(void **state)
{
  db_err_t err;
  link_elem_t elem;
  int spam;

  err = le_init(&elem, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(elem.le_magic, LINK_ELEM_MAGIC);
  assert_ptr_equal(elem.le_next, 0);
  assert_ptr_equal(elem.le_prev, 0);
  assert_ptr_equal(elem.le_object, &spam);
  assert_ptr_equal(elem.le_head, 0);
  assert_int_equal(elem.le_flags, 0);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badargs),
    cmocka_unit_test(test_goodargs)
  };

  return cmocka_run_group_tests_name("Test le_init()", tests, 0, 0);
}
