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

int
rbtree_comp(rb_tree_t *tree, db_key_t *key1, db_key_t *key2)
{
  return 0;
}

void
test_badtree(void **state)
{
  db_err_t err;

  err = rt_init(0, rbtree_comp, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badcomp(void **state)
{
  db_err_t err;
  rb_tree_t tree;

  err = rt_init(&tree, 0, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_goodargs(void **state)
{
  db_err_t err;
  rb_tree_t tree;
  int spam;

  err = rt_init(&tree, rbtree_comp, &spam);

  assert_int_equal(err, 0);
  assert_int_equal(tree.rt_magic, RB_TREE_MAGIC);
  assert_int_equal(tree.rt_flags, 0);
  assert_int_equal(tree.rt_count, 0);
  assert_ptr_equal(tree.rt_root, 0);
  assert_ptr_equal(tree.rt_comp, rbtree_comp);
  assert_ptr_equal(tree.rt_extra, &spam);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_badtree),
    cmocka_unit_test(test_badcomp),
    cmocka_unit_test(test_goodargs)
  };

  return cmocka_run_group_tests_name("Test rt_init.c", tests, 0, 0);
}
