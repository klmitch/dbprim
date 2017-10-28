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
CODE_COVERAGE_IGNORE_PATTERN = tests/* new-tests/* /usr/include/*

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
LDADD = $(top_builddir)/libdbprim.la -lcmocka
check_PROGRAMS = \
	new-tests/test_le_init
TESTS = $(check_PROGRAMS)
