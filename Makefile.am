## Prefered emacs editing mode: -*- Makefile -*-
##
## Copyright (C) 2002 by Kevin L. Mitchell <klmitch@mit.edu>
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
## @(#)$Id$
##
## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = foreign

# The . here forces prefix ordering, so make check rebuilds the
# library first if necessary
SUBDIRS = . tests

lib_LTLIBRARIES = libdbprim.la
libdbprim_la_SOURCES = dbprim.h dbprim_err.h dbprim_int.h dbprim_version.c \
		       \
		       _hash_prime.c he_init.c ht_add.c ht_find.c ht_flush.c \
		       ht_free.c ht_init.c ht_iter.c ht_move.c ht_remove.c \
		       ht_resize.c \
		       \
		       le_init.c ll_add.c ll_find.c ll_flush.c ll_init.c \
		       ll_iter.c ll_move.c ll_remove.c \
		       \
		       _rb_locate.c _rb_rotate.c rn_init.c rt_add.c rt_find.c \
		       rt_flush.c rt_init.c rt_iter.c rt_move.c rt_next.c \
		       rt_remove.c \
		       \
		       sh_find.c sh_flush.c sh_init.c sh_iter.c sh_move.c \
		       _smat_comp.c smat_freelist.c _smat_hash.c \
		       _smat_resize.c st_add.c st_find.c st_flush.c st_free.c \
		       st_init.c st_iter.c st_remove.c st_resize.c
libdbprim_la_LIBADD = @DBPRIM_ET_OBJS@
libdbprim_la_DEPENDENCIES = @DBPRIM_ET_OBJS@
libdbprim_la_LDFLAGS = -version-info 2:0:0

pkginclude_HEADERS = dbprim.h dbprim_err.h dbprim_version.h

bin_SCRIPTS = dbprim-config

BUILT_SOURCES = dbprim_err.h

docmandir = doc/man/man3

man_MANS = $(docmandir)/dbprim_hash.3 \
	   $(docmandir)/dbprim.3 \
	   $(docmandir)/dbprim_link.3 \
	   $(docmandir)/dbprim_rbtree.3 \
	   $(docmandir)/dbprim_smat.3 \
	   \
	   $(docmandir)/DB_KEY_INIT.3 \
	   $(docmandir)/db_key_t.3 \
	   $(docmandir)/dk_key.3 \
	   $(docmandir)/dk_len.3 \
	   $(docmandir)/hash_comp_t.3 \
	   $(docmandir)/HASH_ENTRY_INIT.3 \
	   $(docmandir)/hash_entry_t.3 \
	   $(docmandir)/hash_func_t.3 \
	   $(docmandir)/hash_iter_t.3 \
	   $(docmandir)/hash_resize_t.3 \
	   $(docmandir)/HASH_TABLE_INIT.3 \
	   $(docmandir)/hash_table_t.3 \
	   $(docmandir)/he_flags.3 \
	   $(docmandir)/he_hash.3 \
	   $(docmandir)/he_init.3 \
	   $(docmandir)/he_key.3 \
	   $(docmandir)/he_link.3 \
	   $(docmandir)/he_table.3 \
	   $(docmandir)/he_value.3 \
	   $(docmandir)/he_verify.3 \
	   $(docmandir)/ht_add.3 \
	   $(docmandir)/ht_comp.3 \
	   $(docmandir)/ht_count.3 \
	   $(docmandir)/ht_extra.3 \
	   $(docmandir)/ht_find.3 \
	   $(docmandir)/ht_flags.3 \
	   $(docmandir)/ht_flush.3 \
	   $(docmandir)/ht_free.3 \
	   $(docmandir)/ht_frozen.3 \
	   $(docmandir)/ht_func.3 \
	   $(docmandir)/ht_init.3 \
	   $(docmandir)/ht_iter.3 \
	   $(docmandir)/ht_modulus.3 \
	   $(docmandir)/ht_move.3 \
	   $(docmandir)/ht_remove.3 \
	   $(docmandir)/ht_resize.3 \
	   $(docmandir)/ht_rsize.3 \
	   $(docmandir)/ht_size.3 \
	   $(docmandir)/ht_verify.3 \
	   $(docmandir)/le_flags.3 \
	   $(docmandir)/le_head.3 \
	   $(docmandir)/le_init.3 \
	   $(docmandir)/le_next.3 \
	   $(docmandir)/le_object.3 \
	   $(docmandir)/le_prev.3 \
	   $(docmandir)/le_verify.3 \
	   $(docmandir)/link_comp_t.3 \
	   $(docmandir)/LINK_ELEM_INIT.3 \
	   $(docmandir)/link_elem_t.3 \
	   $(docmandir)/LINK_HEAD_INIT.3 \
	   $(docmandir)/link_head_t.3 \
	   $(docmandir)/link_iter_t.3 \
	   $(docmandir)/link_loc_t.3 \
	   $(docmandir)/ll_add.3 \
	   $(docmandir)/ll_count.3 \
	   $(docmandir)/ll_extra.3 \
	   $(docmandir)/ll_find.3 \
	   $(docmandir)/ll_first.3 \
	   $(docmandir)/ll_flush.3 \
	   $(docmandir)/ll_init.3 \
	   $(docmandir)/ll_iter.3 \
	   $(docmandir)/ll_last.3 \
	   $(docmandir)/ll_move.3 \
	   $(docmandir)/ll_remove.3 \
	   $(docmandir)/ll_verify.3 \
	   $(docmandir)/rb_color_t.3 \
	   $(docmandir)/rb_comp_t.3 \
	   $(docmandir)/rb_iter_t.3 \
	   $(docmandir)/RB_NODE_INIT.3 \
	   $(docmandir)/rb_node_t.3 \
	   $(docmandir)/RB_TREE_INIT.3 \
	   $(docmandir)/rb_tree_t.3 \
	   $(docmandir)/rn_color.3 \
	   $(docmandir)/rn_init.3 \
	   $(docmandir)/rn_isblack.3 \
	   $(docmandir)/rn_isleft.3 \
	   $(docmandir)/rn_isred.3 \
	   $(docmandir)/rn_isright.3 \
	   $(docmandir)/rn_key.3 \
	   $(docmandir)/rn_left.3 \
	   $(docmandir)/rn_parent.3 \
	   $(docmandir)/rn_right.3 \
	   $(docmandir)/rn_tree.3 \
	   $(docmandir)/rn_value.3 \
	   $(docmandir)/rn_verify.3 \
	   $(docmandir)/rt_add.3 \
	   $(docmandir)/rt_comp.3 \
	   $(docmandir)/rt_count.3 \
	   $(docmandir)/rt_extra.3 \
	   $(docmandir)/rt_find.3 \
	   $(docmandir)/rt_flush.3 \
	   $(docmandir)/rt_frozen.3 \
	   $(docmandir)/rt_init.3 \
	   $(docmandir)/rt_iter.3 \
	   $(docmandir)/rt_move.3 \
	   $(docmandir)/rt_next.3 \
	   $(docmandir)/rt_prev.3 \
	   $(docmandir)/rt_remove.3 \
	   $(docmandir)/rt_root.3 \
	   $(docmandir)/rt_verify.3 \
	   $(docmandir)/se_flags.3 \
	   $(docmandir)/se_hash.3 \
	   $(docmandir)/se_lflags.3 \
	   $(docmandir)/se_next.3 \
	   $(docmandir)/se_object.3 \
	   $(docmandir)/se_prev.3 \
	   $(docmandir)/se_table.3 \
	   $(docmandir)/se_verify.3 \
	   $(docmandir)/sh_count.3 \
	   $(docmandir)/sh_elem.3 \
	   $(docmandir)/sh_find.3 \
	   $(docmandir)/sh_first.3 \
	   $(docmandir)/sh_frozen.3 \
	   $(docmandir)/sh_init.3 \
	   $(docmandir)/sh_iter.3 \
	   $(docmandir)/sh_last.3 \
	   $(docmandir)/sh_move.3 \
	   $(docmandir)/sh_object.3 \
	   $(docmandir)/sh_size.3 \
	   $(docmandir)/sh_table.3 \
	   $(docmandir)/sh_verify.3 \
	   $(docmandir)/smat_cleanup.3 \
	   $(docmandir)/smat_comp_t.3 \
	   $(docmandir)/smat_entry_t.3 \
	   $(docmandir)/smat_freemem.3 \
	   $(docmandir)/SMAT_HEAD_INIT.3 \
	   $(docmandir)/smat_head_t.3 \
	   $(docmandir)/smat_iter_t.3 \
	   $(docmandir)/smat_loc_t.3 \
	   $(docmandir)/smat_resize_t.3 \
	   $(docmandir)/SMAT_TABLE_INIT.3 \
	   $(docmandir)/smat_table_t.3 \
	   $(docmandir)/st_add.3 \
	   $(docmandir)/st_count.3 \
	   $(docmandir)/st_extra.3 \
	   $(docmandir)/st_find.3 \
	   $(docmandir)/st_flags.3 \
	   $(docmandir)/st_flush.3 \
	   $(docmandir)/st_free.3 \
	   $(docmandir)/st_frozen.3 \
	   $(docmandir)/st_init.3 \
	   $(docmandir)/st_iter.3 \
	   $(docmandir)/st_modulus.3 \
	   $(docmandir)/st_remove.3 \
	   $(docmandir)/st_resize.3 \
	   $(docmandir)/st_rsize.3 \
	   $(docmandir)/st_size.3 \
	   $(docmandir)/st_verify.3

EXTRA_DIST = dbprim_err.et dbprim.spec dbprim.spec.in \
	     comp_et-sh.in COPYING INSTALL doc

SUFFIXES = .et .h

CLEANFILES = dbprim.h dbprim_version.c dbprim_version.h \
	     dbprim_err.c dbprim_err.h

DISTCLEANFILES = dbprim-config.in

DISTHOOKCLEANFILES = dbprim_err.c \
		     dbprim_err.h \
		     dbprim_version.c \
		     dbprim_version.h \
		     dbprim.h

.et.c .et.h: comp_et-sh
	$(SHELL) comp_et-sh --com_err=@COM_ERR@ --dir=$(top_builddir) $<

dbprim_err.c dbprim_err.h: dbprim_err.et

dist-hook:
	for file in $(DISTHOOKCLEANFILES) ; do \
	    rm -f $(distdir)/$$file ; \
	done
