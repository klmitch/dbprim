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

#include "redblack_int.h"

void
test_badargs(void **state)
{
  db_err_t err;

  err = rn_init(0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_goodargs(void **state)
{
  db_err_t err;
  rb_node_t node;
  int spam;

  err = rn_init(&node, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(node.rn_magic, RB_NODE_MAGIC);
  assert_int_equal(node.rn_color, RB_COLOR_NONE);
  assert_ptr_equal(node.rn_tree, 0);
  assert_ptr_equal(node.rn_parent, 0);
  assert_ptr_equal(node.rn_left, 0);
  assert_ptr_equal(node.rn_right, 0);
  assert_ptr_equal(node.rn_key.dk_key, 0);
  assert_int_equal(node.rn_key.dk_len, 0);
  assert_ptr_equal(node.rn_value, &spam);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badargs),
    cmocka_unit_test(test_goodargs)
  };

  return cmocka_run_group_tests_name("Test rn_init.c", tests, 0, 0);
}
