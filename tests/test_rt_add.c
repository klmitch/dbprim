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

rb_node_t *
__wrap__rb_locate(rb_tree_t *tree, rb_node_t *node, db_key_t *key)
{
  check_expected_ptr(tree);
  check_expected_ptr(node);
  check_expected_ptr(key);
  return (rb_node_t *)mock();
}

void
__wrap__rb_rotate(rb_tree_t *tree, rb_node_t *child)
{
  check_expected_ptr(tree);
  check_expected_ptr(child);
  return (void)mock();
}

void
test_zerotree(void **state)
{
  db_err_t err;
  rb_node_t node = RB_NODE_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = rt_add(0, &node, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badtree(void **state)
{
  db_err_t err;
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t node = RB_NODE_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  tree.rt_magic = 0;

  err = rt_add(&tree, &node, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zeronode(void **state)
{
  db_err_t err;
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  err = rt_add(&tree, 0, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_badnode(void **state)
{
  db_err_t err;
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t node = RB_NODE_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  node.rn_magic = 0;

  err = rt_add(&tree, &node, &key);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_zerokey(void **state)
{
  db_err_t err;
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t node = RB_NODE_INIT(0);

  err = rt_add(&tree, &node, 0);

  assert_int_equal(err, DB_ERR_BADARGS);
}

void
test_busy(void **state)
{
  db_err_t err;
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t node = RB_NODE_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  node.rn_tree = &tree;

  err = rt_add(&tree, &node, &key);

  assert_int_equal(err, DB_ERR_BUSY);
}

void
test_frozen(void **state)
{
  db_err_t err;
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t node = RB_NODE_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  tree.rt_flags |= RBT_FLAG_FREEZE;

  err = rt_add(&tree, &node, &key);

  assert_int_equal(err, DB_ERR_FROZEN);
}

void
test_duplicate(void **state)
{
  db_err_t err;
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t node = RB_NODE_INIT(0);
  rb_node_t other = RB_NODE_INIT(0);
  db_key_t key = DB_KEY_INIT("spam", 4);

  will_return(__wrap__rb_locate, &other);
  expect_value(__wrap__rb_locate, tree, &tree);
  expect_value(__wrap__rb_locate, node, &node);
  expect_value(__wrap__rb_locate, key, &key);

  err = rt_add(&tree, &node, &key);

  assert_int_equal(err, DB_ERR_DUPLICATE);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_zerotree),
    cmocka_unit_test(test_badtree),
    cmocka_unit_test(test_zeronode),
    cmocka_unit_test(test_badnode),
    cmocka_unit_test(test_zerokey),
    cmocka_unit_test(test_busy),
    cmocka_unit_test(test_frozen),
    cmocka_unit_test(test_duplicate)
  };

  return cmocka_run_group_tests_name("Test rt_add.c", tests, 0, 0);
}
