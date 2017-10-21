dnl Copyright (C) 2002, 2006, 2017 by Kevin L. Mitchell <klmitch@mit.edu>
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Library General Public
dnl License as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Library General Public License for more details.
dnl
dnl You should have received a copy of the GNU Library General Public
dnl License along with this library; if not, write to the Free
dnl Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl MA 02111-1307, USA
dnl
dnl Process this file with autoconf to produce a configure script.

AC_INIT([Database Primitives Library], 2.0.0, [klmitch@mit.edu], [dbprim],
	[https://github.com/klmitch/dbprim])
AC_PREREQ(2.64)

AC_COPYRIGHT([
Copyright (C) 2002, 2006, 2017 by Kevin L. Mitchell <klmitch@mit.edu>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA])

AC_CONFIG_SRCDIR([include/dbprim_int.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC_C99
AC_PROG_LIBTOOL

DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(dbprim, Doxyfile, docs)

dnl Checks for libraries.

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_DEFINE_VERSION([dbprim])

AC_CONFIG_FILES([Makefile tests/Makefile Doxyfile dbprim.spec dbprim.pc])

AC_OUTPUT
