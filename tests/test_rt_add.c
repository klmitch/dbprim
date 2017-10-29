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

int
main(void)
{
  const struct CMUnitTest tests[] = {
  };

  return cmocka_run_group_tests_name("Test rt_add.c", tests, 0, 0);
}
