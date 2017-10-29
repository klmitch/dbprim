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
test_noparent(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t child = RB_NODE_INIT(0);
  rb_node_t left = RB_NODE_INIT(0), right = RB_NODE_INIT(0);

  tree.rt_root = &child;
  child.rn_left = &left;
  child.rn_right = &right;
  left.rn_parent = &child;
  right.rn_parent = &child;

  _rb_rotate(&tree, &child);

  assert_ptr_equal(tree.rt_root, &child);
  assert_ptr_equal(child.rn_parent, 0);
  assert_ptr_equal(child.rn_left, &left);
  assert_ptr_equal(child.rn_right, &right);
  assert_ptr_equal(left.rn_parent, &child);
  assert_ptr_equal(right.rn_parent, &child);
}

void
test_rooted_parent_left(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t parent = RB_NODE_INIT(0), child = RB_NODE_INIT(0);
  rb_node_t cleft = RB_NODE_INIT(0), cright = RB_NODE_INIT(0);
  rb_node_t pright = RB_NODE_INIT(0);

  tree.rt_root = &parent;
  parent.rn_left = &child;
  parent.rn_right = &pright;
  child.rn_parent = &parent;
  child.rn_left = &cleft;
  child.rn_right = &cright;
  pright.rn_parent = &parent;
  cleft.rn_parent = &child;
  cright.rn_parent = &child;

  _rb_rotate(&tree, &child);

  assert_ptr_equal(tree.rt_root, &child);
  assert_ptr_equal(parent.rn_parent, &child);
  assert_ptr_equal(parent.rn_left, &cright);
  assert_ptr_equal(parent.rn_right, &pright);
  assert_ptr_equal(child.rn_parent, 0);
  assert_ptr_equal(child.rn_left, &cleft);
  assert_ptr_equal(child.rn_right, &parent);
  assert_ptr_equal(pright.rn_parent, &parent);
  assert_ptr_equal(cleft.rn_parent, &child);
  assert_ptr_equal(cright.rn_parent, &parent);
}

void
test_rooted_parent_left_nograndchild(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t parent = RB_NODE_INIT(0), child = RB_NODE_INIT(0);
  rb_node_t cleft = RB_NODE_INIT(0);
  rb_node_t pright = RB_NODE_INIT(0);

  tree.rt_root = &parent;
  parent.rn_left = &child;
  parent.rn_right = &pright;
  child.rn_parent = &parent;
  child.rn_left = &cleft;
  pright.rn_parent = &parent;
  cleft.rn_parent = &child;

  _rb_rotate(&tree, &child);

  assert_ptr_equal(tree.rt_root, &child);
  assert_ptr_equal(parent.rn_parent, &child);
  assert_ptr_equal(parent.rn_left, 0);
  assert_ptr_equal(parent.rn_right, &pright);
  assert_ptr_equal(child.rn_parent, 0);
  assert_ptr_equal(child.rn_left, &cleft);
  assert_ptr_equal(child.rn_right, &parent);
  assert_ptr_equal(pright.rn_parent, &parent);
  assert_ptr_equal(cleft.rn_parent, &child);
}

void
test_rooted_parent_right(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t parent = RB_NODE_INIT(0), child = RB_NODE_INIT(0);
  rb_node_t cleft = RB_NODE_INIT(0), cright = RB_NODE_INIT(0);
  rb_node_t pleft = RB_NODE_INIT(0);

  tree.rt_root = &parent;
  parent.rn_left = &pleft;
  parent.rn_right = &child;
  child.rn_parent = &parent;
  child.rn_left = &cleft;
  child.rn_right = &cright;
  pleft.rn_parent = &parent;
  cleft.rn_parent = &child;
  cright.rn_parent = &child;

  _rb_rotate(&tree, &child);

  assert_ptr_equal(tree.rt_root, &child);
  assert_ptr_equal(parent.rn_parent, &child);
  assert_ptr_equal(parent.rn_left, &pleft);
  assert_ptr_equal(parent.rn_right, &cleft);
  assert_ptr_equal(child.rn_parent, 0);
  assert_ptr_equal(child.rn_left, &parent);
  assert_ptr_equal(child.rn_right, &cright);
  assert_ptr_equal(pleft.rn_parent, &parent);
  assert_ptr_equal(cleft.rn_parent, &parent);
  assert_ptr_equal(cright.rn_parent, &child);
}

void
test_rooted_parent_right_nograndchild(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t parent = RB_NODE_INIT(0), child = RB_NODE_INIT(0);
  rb_node_t cright = RB_NODE_INIT(0);
  rb_node_t pleft = RB_NODE_INIT(0);

  tree.rt_root = &parent;
  parent.rn_left = &pleft;
  parent.rn_right = &child;
  child.rn_parent = &parent;
  child.rn_right = &cright;
  pleft.rn_parent = &parent;
  cright.rn_parent = &child;

  _rb_rotate(&tree, &child);

  assert_ptr_equal(tree.rt_root, &child);
  assert_ptr_equal(parent.rn_parent, &child);
  assert_ptr_equal(parent.rn_left, &pleft);
  assert_ptr_equal(parent.rn_right, 0);
  assert_ptr_equal(child.rn_parent, 0);
  assert_ptr_equal(child.rn_left, &parent);
  assert_ptr_equal(child.rn_right, &cright);
  assert_ptr_equal(pleft.rn_parent, &parent);
  assert_ptr_equal(cright.rn_parent, &child);
}

void
test_left_parent_left_nograndchild(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t root = RB_NODE_INIT(0);
  rb_node_t parent = RB_NODE_INIT(0), child = RB_NODE_INIT(0);
  rb_node_t cleft = RB_NODE_INIT(0);
  rb_node_t pright = RB_NODE_INIT(0);

  tree.rt_root = &root;
  root.rn_left = &parent;
  parent.rn_parent = &root;
  parent.rn_left = &child;
  parent.rn_right = &pright;
  child.rn_parent = &parent;
  child.rn_left = &cleft;
  pright.rn_parent = &parent;
  cleft.rn_parent = &child;

  _rb_rotate(&tree, &child);

  assert_ptr_equal(tree.rt_root, &root);
  assert_ptr_equal(root.rn_left, &child);
  assert_ptr_equal(parent.rn_parent, &child);
  assert_ptr_equal(parent.rn_left, 0);
  assert_ptr_equal(parent.rn_right, &pright);
  assert_ptr_equal(child.rn_parent, &root);
  assert_ptr_equal(child.rn_left, &cleft);
  assert_ptr_equal(child.rn_right, &parent);
  assert_ptr_equal(pright.rn_parent, &parent);
  assert_ptr_equal(cleft.rn_parent, &child);
}

void
test_right_parent_right_nograndchild(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(0, 0);
  rb_node_t root = RB_NODE_INIT(0);
  rb_node_t parent = RB_NODE_INIT(0), child = RB_NODE_INIT(0);
  rb_node_t cright = RB_NODE_INIT(0);
  rb_node_t pleft = RB_NODE_INIT(0);

  tree.rt_root = &root;
  root.rn_right = &parent;
  parent.rn_parent = &root;
  parent.rn_left = &pleft;
  parent.rn_right = &child;
  child.rn_parent = &parent;
  child.rn_right = &cright;
  pleft.rn_parent = &parent;
  cright.rn_parent = &child;

  _rb_rotate(&tree, &child);

  assert_ptr_equal(tree.rt_root, &root);
  assert_ptr_equal(root.rn_right, &child);
  assert_ptr_equal(parent.rn_parent, &child);
  assert_ptr_equal(parent.rn_left, &pleft);
  assert_ptr_equal(parent.rn_right, 0);
  assert_ptr_equal(child.rn_parent, &root);
  assert_ptr_equal(child.rn_left, &parent);
  assert_ptr_equal(child.rn_right, &cright);
  assert_ptr_equal(pleft.rn_parent, &parent);
  assert_ptr_equal(cright.rn_parent, &child);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_noparent),
    cmocka_unit_test(test_rooted_parent_left),
    cmocka_unit_test(test_rooted_parent_left_nograndchild),
    cmocka_unit_test(test_rooted_parent_right),
    cmocka_unit_test(test_rooted_parent_right_nograndchild),
    cmocka_unit_test(test_left_parent_left_nograndchild),
    cmocka_unit_test(test_right_parent_right_nograndchild)
  };

  return cmocka_run_group_tests_name("Test _rb_rotate.c", tests, 0, 0);
}
