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
test_empty_nocreate(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t *result;
  db_key_t key = DB_KEY_INIT("spam", 4);

  result = _rb_locate(&tree, 0, &key);

  assert_ptr_equal(result, 0);
  assert_ptr_equal(tree.rt_root, 0);
  assert_int_equal(tree.rt_count, 0);
}

void
test_empty_create(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t node = RB_NODE_INIT(0), *result;
  db_key_t key = DB_KEY_INIT("spam", 4);

  result = _rb_locate(&tree, &node, &key);

  assert_ptr_equal(result, &node);
  assert_ptr_equal(tree.rt_root, &node);
  assert_int_equal(tree.rt_count, 1);
  assert_int_equal(node.rn_color, RB_COLOR_BLACK);
  assert_ptr_equal(node.rn_tree, &tree);
  assert_ptr_equal(node.rn_parent, 0);
  assert_ptr_equal(node.rn_left, 0);
  assert_ptr_equal(node.rn_right, 0);
  assert_ptr_equal(node.rn_key.dk_key, key.dk_key);
  assert_int_equal(node.rn_key.dk_len, key.dk_len);
}

void
test_exists_root(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t root = RB_NODE_INIT(0), node = RB_NODE_INIT(0), *result;
  db_key_t key = DB_KEY_INIT("spam", 4);

  tree.rt_root = &root;
  tree.rt_count = 1;
  root.rn_color = RB_COLOR_BLACK;
  root.rn_tree = &tree;
  root.rn_key = key;

  result = _rb_locate(&tree, &node, &key);

  assert_ptr_equal(result, &root);
  assert_ptr_equal(tree.rt_root, &root);
  assert_int_equal(tree.rt_count, 1);
  assert_int_equal(node.rn_color, RB_COLOR_NONE);
  assert_ptr_equal(node.rn_tree, 0);
  assert_ptr_equal(node.rn_parent, 0);
  assert_ptr_equal(node.rn_left, 0);
  assert_ptr_equal(node.rn_right, 0);
  assert_ptr_equal(node.rn_key.dk_key, 0);
  assert_int_equal(node.rn_key.dk_len, 0);
}

void
test_find_left(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t root = RB_NODE_INIT(0), node = RB_NODE_INIT(0), *result;
  rb_node_t rank1_0 = RB_NODE_INIT(0), rank2_1 = RB_NODE_INIT(0);
  db_key_t aaaa = DB_KEY_INIT("aaaa", 4), aaab = DB_KEY_INIT("aaab", 4);
  db_key_t aaac = DB_KEY_INIT("aaac", 4), aaae = DB_KEY_INIT("aaae", 4);

  tree.rt_root = &root;
  tree.rt_count = 4;
  root.rn_color = RB_COLOR_BLACK;
  root.rn_tree = &tree;
  root.rn_key = aaae;
  root.rn_left = &rank1_0;
  rank1_0.rn_color = RB_COLOR_RED;
  rank1_0.rn_tree = &tree;
  rank1_0.rn_parent = &root;
  rank1_0.rn_key = aaaa;
  rank1_0.rn_right = &rank2_1;
  rank2_1.rn_color = RB_COLOR_BLACK;
  rank2_1.rn_tree = &tree;
  rank2_1.rn_parent = &rank1_0;
  rank2_1.rn_left = &node;
  rank2_1.rn_key = aaac;
  node.rn_color = RB_COLOR_RED;
  node.rn_tree = &tree;
  node.rn_parent = &rank2_1;
  node.rn_key = aaab;

  result = _rb_locate(&tree, 0, &aaab);

  assert_ptr_equal(result, &node);
  assert_ptr_equal(tree.rt_root, &root);
  assert_int_equal(tree.rt_count, 4);
  assert_ptr_equal(root.rn_parent, 0);
  assert_ptr_equal(root.rn_left, &rank1_0);
  assert_ptr_equal(root.rn_right, 0);
  assert_ptr_equal(rank1_0.rn_parent, &root);
  assert_ptr_equal(rank1_0.rn_left, 0);
  assert_ptr_equal(rank1_0.rn_right, &rank2_1);
  assert_ptr_equal(rank2_1.rn_parent, &rank1_0);
  assert_ptr_equal(rank2_1.rn_left, &node);
  assert_ptr_equal(rank2_1.rn_right, 0);
  assert_int_equal(node.rn_color, RB_COLOR_RED);
  assert_ptr_equal(node.rn_tree, &tree);
  assert_ptr_equal(node.rn_parent, &rank2_1);
  assert_ptr_equal(node.rn_left, 0);
  assert_ptr_equal(node.rn_right, 0);
  assert_ptr_equal(node.rn_key.dk_key, aaab.dk_key);
  assert_int_equal(node.rn_key.dk_len, aaab.dk_len);
}

void
test_notfound_left(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t root = RB_NODE_INIT(0), *result;
  rb_node_t rank1_0 = RB_NODE_INIT(0), rank2_1 = RB_NODE_INIT(0);
  db_key_t aaaa = DB_KEY_INIT("aaaa", 4), aaab = DB_KEY_INIT("aaab", 4);
  db_key_t aaac = DB_KEY_INIT("aaac", 4), aaae = DB_KEY_INIT("aaae", 4);

  tree.rt_root = &root;
  tree.rt_count = 4;
  root.rn_color = RB_COLOR_BLACK;
  root.rn_tree = &tree;
  root.rn_key = aaae;
  root.rn_left = &rank1_0;
  rank1_0.rn_color = RB_COLOR_RED;
  rank1_0.rn_tree = &tree;
  rank1_0.rn_parent = &root;
  rank1_0.rn_key = aaaa;
  rank1_0.rn_right = &rank2_1;
  rank2_1.rn_color = RB_COLOR_BLACK;
  rank2_1.rn_tree = &tree;
  rank2_1.rn_parent = &rank1_0;
  rank2_1.rn_left = 0;
  rank2_1.rn_key = aaac;

  result = _rb_locate(&tree, 0, &aaab);

  assert_ptr_equal(result, 0);
  assert_ptr_equal(tree.rt_root, &root);
  assert_int_equal(tree.rt_count, 4);
  assert_ptr_equal(root.rn_parent, 0);
  assert_ptr_equal(root.rn_left, &rank1_0);
  assert_ptr_equal(root.rn_right, 0);
  assert_ptr_equal(rank1_0.rn_parent, &root);
  assert_ptr_equal(rank1_0.rn_left, 0);
  assert_ptr_equal(rank1_0.rn_right, &rank2_1);
  assert_ptr_equal(rank2_1.rn_parent, &rank1_0);
  assert_ptr_equal(rank2_1.rn_left, 0);
  assert_ptr_equal(rank2_1.rn_right, 0);
}

void
test_find_right(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t root = RB_NODE_INIT(0), node = RB_NODE_INIT(0), *result;
  rb_node_t rank1_0 = RB_NODE_INIT(0), rank2_1 = RB_NODE_INIT(0);
  db_key_t aaaa = DB_KEY_INIT("aaaa", 4), aaad = DB_KEY_INIT("aaad", 4);
  db_key_t aaac = DB_KEY_INIT("aaac", 4), aaae = DB_KEY_INIT("aaae", 4);

  tree.rt_root = &root;
  tree.rt_count = 4;
  root.rn_color = RB_COLOR_BLACK;
  root.rn_tree = &tree;
  root.rn_key = aaae;
  root.rn_left = &rank1_0;
  rank1_0.rn_color = RB_COLOR_RED;
  rank1_0.rn_tree = &tree;
  rank1_0.rn_parent = &root;
  rank1_0.rn_key = aaaa;
  rank1_0.rn_right = &rank2_1;
  rank2_1.rn_color = RB_COLOR_BLACK;
  rank2_1.rn_tree = &tree;
  rank2_1.rn_parent = &rank1_0;
  rank2_1.rn_right = &node;
  rank2_1.rn_key = aaac;
  node.rn_color = RB_COLOR_RED;
  node.rn_tree = &tree;
  node.rn_parent = &rank2_1;
  node.rn_key = aaad;

  result = _rb_locate(&tree, 0, &aaad);

  assert_ptr_equal(result, &node);
  assert_ptr_equal(tree.rt_root, &root);
  assert_int_equal(tree.rt_count, 4);
  assert_ptr_equal(root.rn_parent, 0);
  assert_ptr_equal(root.rn_left, &rank1_0);
  assert_ptr_equal(root.rn_right, 0);
  assert_ptr_equal(rank1_0.rn_parent, &root);
  assert_ptr_equal(rank1_0.rn_left, 0);
  assert_ptr_equal(rank1_0.rn_right, &rank2_1);
  assert_ptr_equal(rank2_1.rn_parent, &rank1_0);
  assert_ptr_equal(rank2_1.rn_left, 0);
  assert_ptr_equal(rank2_1.rn_right, &node);
  assert_int_equal(node.rn_color, RB_COLOR_RED);
  assert_ptr_equal(node.rn_tree, &tree);
  assert_ptr_equal(node.rn_parent, &rank2_1);
  assert_ptr_equal(node.rn_left, 0);
  assert_ptr_equal(node.rn_right, 0);
  assert_ptr_equal(node.rn_key.dk_key, aaad.dk_key);
  assert_int_equal(node.rn_key.dk_len, aaad.dk_len);
}

void
test_notfound_right(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t root = RB_NODE_INIT(0), *result;
  rb_node_t rank1_0 = RB_NODE_INIT(0), rank2_1 = RB_NODE_INIT(0);
  db_key_t aaaa = DB_KEY_INIT("aaaa", 4), aaad = DB_KEY_INIT("aaad", 4);
  db_key_t aaac = DB_KEY_INIT("aaac", 4), aaae = DB_KEY_INIT("aaae", 4);

  tree.rt_root = &root;
  tree.rt_count = 4;
  root.rn_color = RB_COLOR_BLACK;
  root.rn_tree = &tree;
  root.rn_key = aaae;
  root.rn_left = &rank1_0;
  rank1_0.rn_color = RB_COLOR_RED;
  rank1_0.rn_tree = &tree;
  rank1_0.rn_parent = &root;
  rank1_0.rn_key = aaaa;
  rank1_0.rn_right = &rank2_1;
  rank2_1.rn_color = RB_COLOR_BLACK;
  rank2_1.rn_tree = &tree;
  rank2_1.rn_parent = &rank1_0;
  rank2_1.rn_right = 0;
  rank2_1.rn_key = aaac;

  result = _rb_locate(&tree, 0, &aaad);

  assert_ptr_equal(result, 0);
  assert_ptr_equal(tree.rt_root, &root);
  assert_int_equal(tree.rt_count, 4);
  assert_ptr_equal(root.rn_parent, 0);
  assert_ptr_equal(root.rn_left, &rank1_0);
  assert_ptr_equal(root.rn_right, 0);
  assert_ptr_equal(rank1_0.rn_parent, &root);
  assert_ptr_equal(rank1_0.rn_left, 0);
  assert_ptr_equal(rank1_0.rn_right, &rank2_1);
  assert_ptr_equal(rank2_1.rn_parent, &rank1_0);
  assert_ptr_equal(rank2_1.rn_left, 0);
  assert_ptr_equal(rank2_1.rn_right, 0);
}

void
test_add_left(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t root = RB_NODE_INIT(0), node = RB_NODE_INIT(0), *result;
  rb_node_t rank1_0 = RB_NODE_INIT(0), rank2_1 = RB_NODE_INIT(0);
  db_key_t aaaa = DB_KEY_INIT("aaaa", 4), aaab = DB_KEY_INIT("aaab", 4);
  db_key_t aaac = DB_KEY_INIT("aaac", 4), aaae = DB_KEY_INIT("aaae", 4);

  tree.rt_root = &root;
  tree.rt_count = 3;
  root.rn_color = RB_COLOR_BLACK;
  root.rn_tree = &tree;
  root.rn_key = aaae;
  root.rn_left = &rank1_0;
  rank1_0.rn_color = RB_COLOR_RED;
  rank1_0.rn_tree = &tree;
  rank1_0.rn_parent = &root;
  rank1_0.rn_key = aaaa;
  rank1_0.rn_right = &rank2_1;
  rank2_1.rn_color = RB_COLOR_BLACK;
  rank2_1.rn_tree = &tree;
  rank2_1.rn_parent = &rank1_0;
  rank2_1.rn_key = aaac;

  result = _rb_locate(&tree, &node, &aaab);

  assert_ptr_equal(result, &node);
  assert_ptr_equal(tree.rt_root, &root);
  assert_int_equal(tree.rt_count, 4);
  assert_ptr_equal(root.rn_parent, 0);
  assert_ptr_equal(root.rn_left, &rank1_0);
  assert_ptr_equal(root.rn_right, 0);
  assert_ptr_equal(rank1_0.rn_parent, &root);
  assert_ptr_equal(rank1_0.rn_left, 0);
  assert_ptr_equal(rank1_0.rn_right, &rank2_1);
  assert_ptr_equal(rank2_1.rn_parent, &rank1_0);
  assert_ptr_equal(rank2_1.rn_left, &node);
  assert_ptr_equal(rank2_1.rn_right, 0);
  assert_int_equal(node.rn_color, RB_COLOR_RED);
  assert_ptr_equal(node.rn_tree, &tree);
  assert_ptr_equal(node.rn_parent, &rank2_1);
  assert_ptr_equal(node.rn_left, 0);
  assert_ptr_equal(node.rn_right, 0);
  assert_ptr_equal(node.rn_key.dk_key, aaab.dk_key);
  assert_int_equal(node.rn_key.dk_len, aaab.dk_len);
}

void
test_add_right(void **state)
{
  rb_tree_t tree = RB_TREE_INIT(rbtree_comp, 0);
  rb_node_t root = RB_NODE_INIT(0), node = RB_NODE_INIT(0), *result;
  rb_node_t rank1_0 = RB_NODE_INIT(0), rank2_1 = RB_NODE_INIT(0);
  db_key_t aaaa = DB_KEY_INIT("aaaa", 4), aaad = DB_KEY_INIT("aaad", 4);
  db_key_t aaac = DB_KEY_INIT("aaac", 4), aaae = DB_KEY_INIT("aaae", 4);

  tree.rt_root = &root;
  tree.rt_count = 3;
  root.rn_color = RB_COLOR_BLACK;
  root.rn_tree = &tree;
  root.rn_key = aaae;
  root.rn_left = &rank1_0;
  rank1_0.rn_color = RB_COLOR_RED;
  rank1_0.rn_tree = &tree;
  rank1_0.rn_parent = &root;
  rank1_0.rn_key = aaaa;
  rank1_0.rn_right = &rank2_1;
  rank2_1.rn_color = RB_COLOR_BLACK;
  rank2_1.rn_tree = &tree;
  rank2_1.rn_parent = &rank1_0;
  rank2_1.rn_key = aaac;

  result = _rb_locate(&tree, &node, &aaad);

  assert_ptr_equal(result, &node);
  assert_ptr_equal(tree.rt_root, &root);
  assert_int_equal(tree.rt_count, 4);
  assert_ptr_equal(root.rn_parent, 0);
  assert_ptr_equal(root.rn_left, &rank1_0);
  assert_ptr_equal(root.rn_right, 0);
  assert_ptr_equal(rank1_0.rn_parent, &root);
  assert_ptr_equal(rank1_0.rn_left, 0);
  assert_ptr_equal(rank1_0.rn_right, &rank2_1);
  assert_ptr_equal(rank2_1.rn_parent, &rank1_0);
  assert_ptr_equal(rank2_1.rn_left, 0);
  assert_ptr_equal(rank2_1.rn_right, &node);
  assert_int_equal(node.rn_color, RB_COLOR_RED);
  assert_ptr_equal(node.rn_tree, &tree);
  assert_ptr_equal(node.rn_parent, &rank2_1);
  assert_ptr_equal(node.rn_left, 0);
  assert_ptr_equal(node.rn_right, 0);
  assert_ptr_equal(node.rn_key.dk_key, aaad.dk_key);
  assert_int_equal(node.rn_key.dk_len, aaad.dk_len);
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_empty_nocreate),
    cmocka_unit_test(test_empty_create),
    cmocka_unit_test(test_exists_root),
    cmocka_unit_test(test_find_left),
    cmocka_unit_test(test_notfound_left),
    cmocka_unit_test(test_find_right),
    cmocka_unit_test(test_notfound_right),
    cmocka_unit_test(test_add_left),
    cmocka_unit_test(test_add_right)
  };

  return cmocka_run_group_tests_name("Test _rb_locate.c", tests, 0, 0);
}
