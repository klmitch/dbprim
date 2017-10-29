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

#include "sparsemat_int.h"

db_err_t
__wrap_ll_init(link_head_t *list, void *extra)
{
  check_expected_ptr(list);
  check_expected_ptr(extra);
  return (db_err_t)mock();
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
  };

  return cmocka_run_group_tests_name("Test sh_init.c", tests, 0, 0);
}
