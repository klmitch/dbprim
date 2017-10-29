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
__wrap__st_remove(smat_table_t *table, smat_entry_t *entry, db_flag_t remflag)
{
  check_expected_ptr(table);
  check_expected_ptr(entry);
  check_expected(remflag);
  return (db_err_t)mock();
}

void
__wrap__smat_free(smat_entry_t *entry)
{
  check_expected_ptr(entry);
  return (void)mock();
}

db_err_t
__wrap_ht_flush(hash_table_t *table, hash_iter_t flush_func, void *extra)
{
  check_expected_ptr(table);
  check_expected(flush_func);
  check_expected_ptr(extra);
  return (db_err_t)mock();
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
  };

  return cmocka_run_group_tests_name("Test st_flush.c", tests, 0, 0);
}
