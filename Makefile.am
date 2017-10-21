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

# The . here forces prefix ordering, so make check rebuilds the
# library first if necessary
SUBDIRS = . tests

AM_CPPFLAGS = -D__DBPRIM_LIBRARY__ -I$(top_builddir) -I$(top_srcdir)/include

lib_LTLIBRARIES = libdbprim.la
libdbprim_la_SOURCES = \
	include/dbprim.h \
	include/dbprim_int.h \
	\
	src/dbprim_err.c \
	dbprim_version.c \
	\
	src/_hash_prime.c \
	src/hash_fnv1.c \
	src/hash_fnv1a.c \
	src/hash_comp.c \
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
	src/sh_find.c \
	src/sh_flush.c \
	src/sh_init.c \
	src/sh_iter.c \
	src/sh_move.c \
	src/smat_freelist.c \
	src/_smat_resize.c \
	src/st_add.c \
	src/st_find.c \
	src/st_flush.c \
	src/st_free.c \
	src/st_init.c \
	src/st_iter.c \
	src/st_remove.c \
	src/st_resize.c
libdbprim_la_LDFLAGS = -version-info 4:0:0

pkginclude_HEADERS = include/dbprim.h dbprim_version.h

EXTRA_CSTATUS = dbprim_version.c dbprim_version.h

EXTRA_DIST = dbprim.spec dbprim.spec.in m4 \
	     autogen.sh COPYING INSTALL AUTHORS \
	     Doxyfile.in

CLEANFILES = $(EXTRA_CSTATUS)

DISTHOOKCLEANFILES = $(CLEANFILES)

$(EXTRA_CSTATUS): $(top_srcdir)/configure config.status
	cd $(top_builddir) && $(SHELL) ./config.status $@

@DX_RULES@

dist-hook:
	for file in $(DISTHOOKCLEANFILES) ; do \
	    rm -f $(distdir)/$$file ; \
	done ; \
	find $(distdir) -name .git -print -prune | xargs rm -rf
