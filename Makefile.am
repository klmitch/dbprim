## Prefered emacs editing mode: -*- Makefile -*-
##
## Copyright (C) 2002, 2006, 2017 by Kevin L. Mitchell <klmitch@mit.edu>
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Library General Public
## License as published by the Free Software Foundation; either
## version 2 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Library General Public License for more details.
##
## You should have received a copy of the GNU Library General Public
## License along with this library; if not, write to the Free
## Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
## MA 02111-1307, USA
##
## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = subdir-objects foreign

# The code coverage rules use $(PACKAGE_NAME), which contains spaces;
# they should use $(PACKAGE_TARNAME).
CODE_COVERAGE_OUTPUT_FILE = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-coverage.info
CODE_COVERAGE_OUTPUT_DIRECTORY = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-coverage

# Turn on branch coverage
CODE_COVERAGE_BRANCH_COVERAGE = 1

# Ignore coverage in the test suite or any system header files
CODE_COVERAGE_IGNORE_PATTERN = tests/* /usr/include/*

# Set up common flags
AM_LIBS = $(CODE_COVERAGE_LIBS)
AM_CPPFLAGS = \
	-D__DBPRIM_LIBRARY__ -I$(top_builddir) -I$(top_srcdir)/include \
	$(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)

@CODE_COVERAGE_RULES@

lib_LTLIBRARIES = libdbprim.la
libdbprim_la_SOURCES = \
	include/common.h \
	include/common_int.h \
	include/dbprim.h \
	include/hashtab.h \
	include/hashtab_int.h \
	include/linklist.h \
	include/linklist_int.h \
	include/redblack.h \
	include/redblack_int.h \
	include/sparsemat.h \
	include/sparsemat_int.h \
	\
	src/dbprim_err.c \
	dbprim_version.c \
	\
	src/_hash_prime.c \
	src/hash_comp.c \
	src/hash_fnv1.c \
	src/hash_fnv1_accum.c \
	src/hash_fnv1a.c \
	src/hash_fnv1a_accum.c \
	src/hash_fnv_final.c \
	src/hash_fnv_init.c \
	src/he_init.c \
	src/ht_add.c \
	src/ht_find.c \
	src/ht_flush.c \
	src/ht_free.c \
	src/ht_init.c \
	src/ht_iter.c \
	src/ht_move.c \
	src/ht_remove.c \
	src/ht_resize.c \
	\
	src/le_init.c \
	src/ll_add.c \
	src/ll_find.c \
	src/ll_flush.c \
	src/ll_init.c \
	src/ll_iter.c \
	src/ll_move.c \
	src/ll_remove.c \
	\
	src/_rb_locate.c \
	src/_rb_rotate.c \
	src/rbtree_comp.c \
	src/rn_init.c \
	src/rt_add.c \
	src/rt_find.c \
	src/rt_flush.c \
	src/rt_init.c \
	src/rt_iter.c \
	src/rt_move.c \
	src/rt_next.c \
	src/rt_remove.c \
	\
	src/_smat_resize.c \
	src/sh_find.c \
	src/sh_flush.c \
	src/sh_init.c \
	src/sh_iter.c \
	src/sh_move.c \
	src/smat_freelist.c \
	src/st_add.c \
	src/st_find.c \
	src/st_flush.c \
	src/st_free.c \
	src/st_init.c \
	src/st_iter.c \
	src/st_remove.c \
	src/st_resize.c
libdbprim_la_LDFLAGS = -version-info 4:0:0

pkginclude_HEADERS = \
	include/common.h \
	include/dbprim.h \
	dbprim_version.h \
	include/hashtab.h \
	include/linklist.h \
	include/redblack.h \
	include/sparsemat.h

EXTRA_CSTATUS = dbprim_version.c dbprim_version.h

EXTRA_DIST = dbprim.spec dbprim.spec.in m4 \
	     autogen.sh COPYING INSTALL AUTHORS README.rst \
	     Doxyfile.in dbprim.pc.in

CLEANFILES = $(EXTRA_CSTATUS)

DISTHOOKCLEANFILES = $(CLEANFILES)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dbprim.pc

$(EXTRA_CSTATUS): $(top_srcdir)/configure config.status
	cd $(top_builddir) && $(SHELL) ./config.status $@

@DX_RULES@

dist-hook:
	for file in $(DISTHOOKCLEANFILES) ; do \
	    rm -f $(distdir)/$$file ; \
	done ; \
	find $(distdir) -name .git -print -prune | xargs rm -rf

# Testing-related definitions
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
	$(top_srcdir)/autotools/tap-driver.sh
AM_TESTS_ENVIRONMENT = CMOCKA_MESSAGE_OUTPUT=TAP; export CMOCKA_MESSAGE_OUTPUT;
LDADD = -lcmocka
check_PROGRAMS = \
	tests/test__hash_prime \
	tests/test__rb_locate \
	tests/test__rb_rotate \
	tests/test__smat_resize \
	tests/test_dbprim_err \
	tests/test_hash_comp \
	tests/test_hash_fnv1 \
	tests/test_hash_fnv1_accum \
	tests/test_hash_fnv1a \
	tests/test_hash_fnv1a_accum \
	tests/test_hash_fnv_final \
	tests/test_hash_fnv_init \
	tests/test_he_init \
	tests/test_ht_add \
	tests/test_ht_find \
	tests/test_ht_flush \
	tests/test_ht_free \
	tests/test_ht_init \
	tests/test_ht_iter \
	tests/test_ht_move \
	tests/test_ht_remove \
	tests/test_ht_resize \
	tests/test_le_init \
	tests/test_ll_add \
	tests/test_ll_find \
	tests/test_ll_flush \
	tests/test_ll_init \
	tests/test_ll_iter \
	tests/test_ll_move \
	tests/test_ll_remove \
	tests/test_rbtree_comp \
	tests/test_rn_init \
	tests/test_rt_add \
	tests/test_rt_find \
	tests/test_rt_flush \
	tests/test_rt_init \
	tests/test_rt_iter \
	tests/test_rt_move \
	tests/test_rt_next \
	tests/test_rt_remove \
	tests/test_sh_find \
	tests/test_sh_flush \
	tests/test_sh_init \
	tests/test_sh_iter \
	tests/test_sh_move \
	tests/test_smat_freelist \
	tests/test_st_add \
	tests/test_st_find \
	tests/test_st_flush \
	tests/test_st_free \
	tests/test_st_init \
	tests/test_st_iter \
	tests/test_st_remove \
	tests/test_st_resize
TESTS = $(check_PROGRAMS)

tests_test__hash_prime_LDFLAGS = src/_hash_prime.lo

tests_test__rb_locate_LDFLAGS = src/_rb_locate.lo \
	src/rbtree_comp.lo

tests_test__rb_rotate_LDFLAGS = src/_rb_rotate.lo

tests_test__smat_resize_LDFLAGS = src/_smat_resize.lo

tests_test_dbprim_err_LDFLAGS = src/dbprim_err.lo

tests_test_hash_comp_LDFLAGS = src/hash_comp.lo

tests_test_hash_fnv1_LDFLAGS = src/hash_fnv1.lo \
	-Wl,--wrap=hash_fnv_init \
	-Wl,--wrap=hash_fnv1_accum \
	-Wl,--wrap=hash_fnv_final

tests_test_hash_fnv1_accum_LDFLAGS = src/hash_fnv1_accum.lo

tests_test_hash_fnv1a_LDFLAGS = src/hash_fnv1a.lo \
	-Wl,--wrap=hash_fnv_init \
	-Wl,--wrap=hash_fnv1a_accum \
	-Wl,--wrap=hash_fnv_final

tests_test_hash_fnv1a_accum_LDFLAGS = src/hash_fnv1a_accum.lo

tests_test_hash_fnv_final_LDFLAGS = src/hash_fnv_final.lo

tests_test_hash_fnv_init_LDFLAGS = src/hash_fnv_init.lo

tests_test_he_init_LDFLAGS = src/he_init.lo \
	-Wl,--wrap=le_init

tests_test_ht_add_LDFLAGS = src/ht_add.lo \
	src/hash_comp.lo \
	src/hash_fnv1_accum.lo \
	src/hash_fnv1.lo \
	src/hash_fnv_final.lo \
	src/hash_fnv_init.lo \
	-Wl,--wrap=ht_find \
	-Wl,--wrap=ll_add \
	-Wl,--wrap=ht_resize

tests_test_ht_find_LDFLAGS = src/ht_find.lo \
	src/hash_comp.lo \
	src/hash_fnv1_accum.lo \
	src/hash_fnv1.lo \
	src/hash_fnv_final.lo \
	src/hash_fnv_init.lo

tests_test_ht_flush_LDFLAGS = src/ht_flush.lo \
	-Wl,--wrap=ll_remove \
	-Wl,--wrap=ht_free

tests_test_ht_free_SOURCES = \
	tests/mock_malloc.c \
	tests/mock_malloc.h \
	tests/test_ht_free.c
tests_test_ht_free_LDFLAGS = src/ht_free.lo \
	-Wl,--wrap=malloc \
	-Wl,--wrap=free

tests_test_ht_init_SOURCES = \
	tests/mock_malloc.c \
	tests/mock_malloc.h \
	tests/test_ht_init.c
tests_test_ht_init_LDFLAGS = src/ht_init.lo \
	-Wl,--wrap=malloc \
	-Wl,--wrap=free \
	-Wl,--wrap=_hash_prime \
	-Wl,--wrap=ll_init

tests_test_ht_iter_LDFLAGS = src/ht_iter.lo

tests_test_ht_move_LDFLAGS = src/ht_move.lo \
	-Wl,--wrap=ht_find \
	-Wl,--wrap=ll_remove \
	-Wl,--wrap=ll_add

tests_test_ht_remove_LDFLAGS = src/ht_remove.lo \
	-Wl,--wrap=ll_remove \
	-Wl,--wrap=ht_resize

tests_test_ht_resize_SOURCES = \
	tests/mock_malloc.c \
	tests/mock_malloc.h \
	tests/test_ht_resize.c
tests_test_ht_resize_LDFLAGS = src/ht_resize.lo \
	-Wl,--wrap=malloc \
	-Wl,--wrap=free \
	-Wl,--wrap=_hash_prime \
	-Wl,--wrap=ll_init \
	-Wl,--wrap=ll_add \
	-Wl,--wrap=ll_remove

tests_test_le_init_LDFLAGS = src/le_init.lo

tests_test_ll_add_LDFLAGS = src/ll_add.lo

tests_test_ll_find_LDFLAGS = src/ll_find.lo

tests_test_ll_flush_LDFLAGS = src/ll_flush.lo \
	-Wl,--wrap=ll_remove

tests_test_ll_init_LDFLAGS = src/ll_init.lo

tests_test_ll_iter_LDFLAGS = src/ll_iter.lo

tests_test_ll_move_LDFLAGS = src/ll_move.lo

tests_test_ll_remove_LDFLAGS = src/ll_remove.lo

tests_test_rbtree_comp_LDFLAGS = src/rbtree_comp.lo

tests_test_rn_init_LDFLAGS = src/rn_init.lo

tests_test_rt_add_LDFLAGS = src/rt_add.lo \
	-Wl,--wrap=_rb_locate \
	-Wl,--wrap=_rb_rotate

tests_test_rt_find_LDFLAGS = src/rt_find.lo \
	-Wl,--wrap=_rb_locate

tests_test_rt_flush_LDFLAGS = src/rt_flush.lo \
	-Wl,--wrap=rt_remove

tests_test_rt_init_LDFLAGS = src/rt_init.lo

tests_test_rt_iter_LDFLAGS = src/rt_iter.lo \
	-Wl,--wrap=rt_next

tests_test_rt_move_LDFLAGS = src/rt_move.lo \
	-Wl,--wrap=_rb_locate \
	-Wl,--wrap=rt_remove \
	-Wl,--wrap=rt_add

tests_test_rt_next_LDFLAGS = src/rt_next.lo

tests_test_rt_remove_LDFLAGS = src/rt_remove.lo \
	-Wl,--wrap=_rb_rotate

tests_test_sh_find_LDFLAGS = src/sh_find.lo \
	-Wl,--wrap=ll_find

tests_test_sh_flush_LDFLAGS = src/sh_flush.lo \
	-Wl,--wrap=_st_remove \
	-Wl,--wrap=_smat_free \
	-Wl,--wrap=ll_flush

tests_test_sh_init_LDFLAGS = src/sh_init.lo \
	-Wl,--wrap=ll_init

tests_test_sh_iter_LDFLAGS = src/sh_iter.lo \
	-Wl,--wrap=ll_iter

tests_test_sh_move_LDFLAGS = src/sh_move.lo \
	-Wl,--wrap=ll_move

tests_test_smat_freelist_SOURCES = \
	tests/mock_malloc.c \
	tests/mock_malloc.h \
	tests/test_smat_freelist.c
tests_test_smat_freelist_LDFLAGS = src/smat_freelist.lo \
	-Wl,--wrap=malloc \
	-Wl,--wrap=free \
	-Wl,--wrap=ll_remove \
	-Wl,--wrap=he_init \
	-Wl,--wrap=le_init \
	-Wl,--wrap=ll_add

tests_test_st_add_LDFLAGS = src/st_add.lo \
	-Wl,--wrap=_smat_alloc \
	-Wl,--wrap=ht_add \
	-Wl,--wrap=ll_add \
	-Wl,--wrap=ll_remove \
	-Wl,--wrap=ht_remove \
	-Wl,--wrap=_smat_free

tests_test_st_find_LDFLAGS = src/st_find.lo \
	-Wl,--wrap=ht_find

tests_test_st_flush_LDFLAGS = src/st_flush.lo \
	-Wl,--wrap=_st_remove \
	-Wl,--wrap=_smat_free \
	-Wl,--wrap=ht_flush

tests_test_st_free_LDFLAGS = src/st_free.lo \
	-Wl,--wrap=ht_free

tests_test_st_init_LDFLAGS = src/st_init.lo \
	src/hash_comp.lo \
	src/hash_fnv_init.lo \
	src/hash_fnv1a_accum.lo \
	src/hash_fnv_final.lo \
	src/hash_fnv1a.lo \
	src/_smat_resize.lo \
	-Wl,--wrap=ht_init

tests_test_st_iter_LDFLAGS = src/st_iter.lo \
	-Wl,--wrap=ht_iter

tests_test_st_remove_LDFLAGS = src/st_remove.lo \
	-Wl,--wrap=_smat_free \
	-Wl,--wrap=ht_remove \
	-Wl,--wrap=ll_remove

tests_test_st_resize_LDFLAGS = src/st_resize.lo \
	-Wl,--wrap=ht_resize
