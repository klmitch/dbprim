dnl acinclude.m4 generated automatically by ac-archive's acinclude 0.5.39

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl ______ /home/klmitch/devel/acinclude/com_err-lib.m4 ______
dnl @(#)%Id: com_err-lib.m4,v 1.3 1999/09/02 15:01:17 klmitch Exp %
dnl
dnl KLM_COM_ERR_LIB
dnl
dnl Checks to see if the com_err libraries are locatable

AC_DEFUN(KLM_COM_ERR_LIB,
[AC_REQUIRE([KLM_WITH_COM_ERR])
t_LIBS=${LIBS}
LIBS="${LIBS} -L${klm_cv_path_com_err}/lib"

AC_CHECK_LIB(com_err, com_err, ,
[AC_MSG_ERROR([Unable to find com_err libraries])])

LIBS=${t_LIBS}
COM_ERR_LIBPATH="${klm_cv_path_com_err}/lib"
COM_ERR_LDADD="-L${COM_ERR_LIBPATH} -lcom_err"
AC_SUBST([COM_ERR_LIBPATH])
AC_SUBST([COM_ERR_LDADD])])

dnl ______ /home/klmitch/devel/acinclude/with-com_err.m4 ______
dnl @(#)%Id: with-com_err.m4,v 1.4 2000/12/13 05:01:35 klmitch Exp %
dnl
dnl KLM_WITH_COM_ERR
dnl
dnl Adds --with-com_err, which tells it where to look for com_err

AC_DEFUN(KLM_WITH_COM_ERR,
[klm_path_com_err=no

AC_ARG_WITH(com_err,
[  --with-com_err=dir      Use the com_err installed in dir (/usr/local)],
[if test "x${withval}" = "xyes"; then
    klm_path_com_err=
else
    klm_path_com_err="${withval}"
fi
])

if test "x${klm_path_com_err}" != "xno"; then
    AC_CACHE_CHECK([com_err install location], klm_cv_path_com_err,
    [if test "x${klm_path_com_err}" = x; then
	klm_cv_path_com_err=/usr/local
    else
	klm_cv_path_com_err="${klm_path_com_err}"
    fi
    ])

    COM_ERR=true
else
    klm_cv_path_com_err=
    COM_ERR=false
fi
AC_SUBST(COM_ERR)
])

dnl ______ /home/klmitch/devel/acinclude/com_err-obj.m4 ______
dnl @(#)%Id: com_err-obj.m4,v 1.1 2000/12/13 06:09:27 klmitch Exp %
dnl
dnl KLM_COM_ERR_OBJ
dnl
dnl Defines the given OBJ if com_err is enabled.

AC_DEFUN(KLM_COM_ERR_OBJ,
[AC_REQUIRE([KLM_WITH_COM_ERR])
if test "x$COM_ERR" = "xtrue"; then
    $1="$2"
else
    $1=
fi
AC_SUBST($1)
])

dnl ______ /usr/share/aclocal/rleigh/ac_config_libconfig_in.m4 ______
dnl @synopsis AC_CONFIG_LIBCONFIG_IN [(LIBRARY [, DESTINATION [, MODULES]])]
dnl
dnl Creates a custom LIBRARY-config script.  The script supports
dnl --cflags, --libs and --version options, as well as all of the
dnl standard configure variables e.g. --libdir and --sysconfdir.
dnl --prefix and --exec-prefix may be specified to override the
dnl defaults.  --help gives a complete list of available options.
dnl Modules allow multiple sets of --libs, --cflags and --version
dnl options to be used, and combined together.
dnl
dnl This macro saves you all the typing for a library-config.in script;
dnl you don't even need to distribute one along. Place this macro
dnl in your configure.ac, et voila, you got one that you want to install.
dnl
dnl The options:
dnl
dnl  $1 = LIBRARY       e.g. gtk, ncurses
dnl  $2 = DESTINATION   Directory path, e.g. src/scripts
dnl  $3 = MODULES       Additional library modules, not including the
dnl                     default ($1).
dnl
dnl It is suggested that the following CFLAGS and LIBS variables are
dnl used in your configure.in.  library_libs is *essential*.
dnl library_cflags is important, but not always needed.  If they do not
dnl exist, defaults will be taken from LIBRARY_CFLAGS, LIBRARY_LIBS
dnl (should be -llibrary *only*) and LIBRARY_LIBDEPS (-l options for
dnl libraries your library depends upon.
dnl LIBLIBRARY_LIBS is simply $LIBRARY_LIBS $LIBRARY_LIBDEPS.
dnl NB. LIBRARY and library are the name of your library, in upper and
dnl lower case repectively e.g. GTK, gtk.
dnl
dnl  LIBRARY_CFLAGS:    cflags for compiling libraries and example progs
dnl  LIBRARY_LIBS:      libraries for linking programs
dnl  LIBRARY_LIBDEPS*:  libraries for linking libraries against (needed
dnl                     to link -static
dnl  library_cflags*:   cflags to store in library-config
dnl  library_libs*:     libs to store in library-config
dnl  LIBLIBRARY_LIBS:   libs to link programs IN THIS PACKAGE ONLY against
dnl  LIBRARY_VERSION*:  the version of your library (x.y.z recommended)
dnl    *=required if you want sensible output, otherwise they will be
dnl      *guessed* (DWIM)
dnl
dnl Support for pkg-config is available by default.  libs, cflags and
dnl version information will be obtained from the pkg-config metadata
dnl file library.pc if it exists in the pkgconfig data directory.
dnl The above information still needs to be specified, as it will still
dnl be used if pkg-config is not found on the host system at configure
dnl time.
dnl
dnl -I and -L flags will be obtained from *both* pkgconfig and the script.
dnl AC_CONFIG_LIBCONFIG_IN_PKGCONFIG enables pkg-config support (default)
dnl AC_CONFIG_LIBCONFIG_IN_STATIC disables pkg-config support
dnl
dnl There is also an AC_SUBST(LIBRARY_CONFIG) that will be set to
dnl the name of the file that we output in this macro. Use as:
dnl
dnl  install-exec-local: install-config
dnl  install-config:
dnl     $(mkinstalldirs) $(DESTDIR)$(bindir)
dnl     $(INSTALL_EXEC) @LIBRARY_CONFIG@ $(DESTDIR)$(bindir)
dnl
dnl Or, if using automake:
dnl
dnl  bin_SCRIPTS = @LIBRARY_CONFIG@
dnl
dnl Based upon AC_CREATE_GENERIC_CONFIG (ac_create_generic_config.m4)
dnl by Guido Draheim <guidod@gmx.de> and gtk-config by Owen Taylor.
dnl
dnl @version %Id: ac_config_libconfig_in.m4,v 1.4 2002/09/12 22:11:52 guidod Exp %
dnl @author Roger Leigh <roger@whinlatter.uklinux.net>
dnl
# AC_CONFIG_LIBCONFIG_IN(LIBRARY, DESTINATION, MODULES)
# -----------------------------------------------------
# Generate a custom LIBRARY-config script.  Create the script in the
# DESTINATION directory, including support for MODULES.
AC_DEFUN([AC_CONFIG_LIBCONFIG_IN],
[# create a custom library-config file ($1-config)
m4_if(AC_CONFIG_LIBCONFIG_IN_USEPKGCONFIG, [true],
      [AC_PATH_PROG(PKG_CONFIG, pkg-config)])
pushdef([LIBCONFIG_DIR], [m4_if([$2], , , [$2/])])
LIBCONFIG_FILE="LIBCONFIG_DIR[]$1-config.in"
AC_SUBST(target)dnl
AC_SUBST(host)dnl
AC_SUBST(build)dnl
dnl create directory if it does not preexist
m4_if([$2], , , [AS_MKDIR_P([$2])])
AC_MSG_NOTICE([creating $LIBCONFIG_FILE])
echo '#! /bin/sh' >$LIBCONFIG_FILE
echo "# $1-config library configuration script" >>$LIBCONFIG_FILE
echo '# generated by ac_config_libconfig_in.m4' >>$LIBCONFIG_FILE
echo ' ' >>$LIBCONFIG_FILE
echo 'template_version="1.0.0"' >>$LIBCONFIG_FILE
echo ' ' >>$LIBCONFIG_FILE
echo 'package="@PACKAGE@"' >>$LIBCONFIG_FILE
echo ' ' >>$LIBCONFIG_FILE
echo '# usage instructions if no options given' >>$LIBCONFIG_FILE
echo 'if test "'"\$""#"'" -eq 0; then' >>$LIBCONFIG_FILE
echo '   cat <<EOF' >>$LIBCONFIG_FILE
m4_if($3, ,
      [echo 'Usage: $1-config [[OPTIONS]]' >>$LIBCONFIG_FILE],
      [echo 'Usage: $1-config [[OPTIONS]] [[LIBRARIES]]' >>$LIBCONFIG_FILE])
echo 'Options:' >>$LIBCONFIG_FILE
echo '        [[--prefix[=DIR]]]' >>$LIBCONFIG_FILE
echo '        [[--exec-prefix[=DIR]]]' >>$LIBCONFIG_FILE
echo '        [[--package]]' >>$LIBCONFIG_FILE
echo '        [[--version]]' >>$LIBCONFIG_FILE
echo '        [[--cflags]]' >>$LIBCONFIG_FILE
echo '        [[--libs]]' >>$LIBCONFIG_FILE
echo '        [[--help]]' >>$LIBCONFIG_FILE
m4_if($3, , ,
      [echo 'Libraries:' >>$LIBCONFIG_FILE
       for module in $1 $3 ; do
         echo "        $module" >>$LIBCONFIG_FILE ;
       done])
echo 'EOF' >>$LIBCONFIG_FILE
echo 'fi' >>$LIBCONFIG_FILE
echo ' ' >>$LIBCONFIG_FILE
echo '# parse options' >>$LIBCONFIG_FILE
echo 'o=""' >>$LIBCONFIG_FILE
echo 'h=""' >>$LIBCONFIG_FILE
echo 'for i ; do' >>$LIBCONFIG_FILE
echo '  case $i in' >>$LIBCONFIG_FILE
options="prefix exec-prefix eprefix package version cflags libs bindir sbindir libexecdir datadir sysconfdir sharedstatedir localstatedir libdir infodir mandir target host build pkgdatadir pkglibdir pkgincludedir template-version help"
echo '    --prefix=*) prefix=`echo $i | sed -e "s/--prefix=//"` ;;' >>$LIBCONFIG_FILE
echo '    --exec-prefix=*) exec_prefix=`echo $i | sed -e "s/--exec-prefix=//"` ;;' >>$LIBCONFIG_FILE
echo '    --eprefix=*) exec_prefix=`echo $i | sed -e "s/--eprefix=//"` ;;' >>$LIBCONFIG_FILE
for option in $options ; do
  case $option in
    exec-prefix)  echo "    --$option) echo_exec_prefix=\"yes\" ;;" >>$LIBCONFIG_FILE ;;
    template-version)  echo "    --$option) echo_template_version=\"yes\" ;;" >>$LIBCONFIG_FILE ;;
    *)  echo "    --$option) echo_$option=\"yes\" ;;" >>$LIBCONFIG_FILE ;;
  esac
done
m4_if($3, , ,
      [for module in $1 $3 ; do
         echo "  $module) echo_module_$module=\"yes\" ;" >>$LIBCONFIG_FILE ;
         echo '    echo_module="yes" ;;' >>$LIBCONFIG_FILE ;
       done])
echo '    //*|/*//*|./*//*)        echo_extra="yes" ;;' >>$LIBCONFIG_FILE
echo '    *) eval "echo Unknown option: $i" ; exit 1 ;;' >>$LIBCONFIG_FILE
echo '  esac' >>$LIBCONFIG_FILE
echo 'done' >>$LIBCONFIG_FILE
echo ' ' >>$LIBCONFIG_FILE
# in the order of occurence a standard automake Makefile
echo '# defaults from configure; set only if not set previously' >>$LIBCONFIG_FILE
vars="prefix exec_prefix bindir sbindir libexecdir datadir sysconfdir sharedstatedir localstatedir libdir infodir mandir includedir target host build"
for var in $vars ; do
  echo "if test -z \"\$$var\" ; then" >>$LIBCONFIG_FILE
  echo "  $var=\"@$var@\"" >>$LIBCONFIG_FILE
  echo 'fi' >>$LIBCONFIG_FILE
done
echo ' ' >>$LIBCONFIG_FILE
echo '# generate output' >>$LIBCONFIG_FILE
echo 'if test x$echo_module != xyes ; then' >>$LIBCONFIG_FILE
echo '  echo_module_$1="yes"' >>$LIBCONFIG_FILE
echo 'fi' >>$LIBCONFIG_FILE
AC_CONFIG_LIBCONFIG_IN_MODULES(m4_if([$3], , [$1], [m4_translit([$1 $3], [ ], [,])]))dnl
for option in $options extra; do
  case $option in
    exec-prefix)  echo "if test x\$echo_exec_prefix = xyes ; then" >>$LIBCONFIG_FILE ;;
    template-version)  echo "if test x\$echo_template_version = xyes ; then" >>$LIBCONFIG_FILE ;;
    *)  echo "if test x\$echo_$option = xyes ; then" >>$LIBCONFIG_FILE ;;
  esac
  case $option in
    exec-prefix | eprefix)  echo '  o="$o $exec_prefix"' >>$LIBCONFIG_FILE ;;
    template-version)  echo '  o="$o $template_version"' >>$LIBCONFIG_FILE ;;
    cflags)
      echo '  i=`eval echo "$includedir"`' >>$LIBCONFIG_FILE ;
      echo '  i=`eval echo "$i"`' >>$LIBCONFIG_FILE ;
      echo '  if test "_$i" != "_/usr/include" ; then' >>$LIBCONFIG_FILE ;
      echo '    o="$o -I$includedir"' >>$LIBCONFIG_FILE ;
      echo '  fi' >>$LIBCONFIG_FILE ;
      echo '  o="$o $cflags"' >>$LIBCONFIG_FILE ;;
    libs)  echo '  o="$o -L$libdir $libs"' >>$LIBCONFIG_FILE ;;
    help)  echo '  h="1"' >>$LIBCONFIG_FILE ;;
    pkgdatadir)  echo "  o=\"$o \${datadir}/\${package}\"" >>$LIBCONFIG_FILE ;;
    pkglibdir)  echo "  o=\"$o \${libdir}/\${package}\"" >>$LIBCONFIG_FILE ;;
    pkgincludedir)  echo "  o=\"$o \${includedir}/\${package}\"" >>$LIBCONFIG_FILE ;;
    extra)
      echo '  v=`echo $i | sed -e s://:\$:g`' >>$LIBCONFIG_FILE ;
      echo '  v=`eval "echo $v"`' >>$LIBCONFIG_FILE ;
      echo '  o="$o $v"' >>$LIBCONFIG_FILE ;;
    *)  echo "  o=\"$o \$$option\"" >>$LIBCONFIG_FILE
  esac
  echo 'fi' >>$LIBCONFIG_FILE
done
echo ' ' >>$LIBCONFIG_FILE
echo '# output data' >>$LIBCONFIG_FILE
echo 'o=`eval "echo $o"`' >>$LIBCONFIG_FILE
echo 'o=`eval "echo $o"`' >>$LIBCONFIG_FILE
echo 'if test -n "$o" ; then ' >>$LIBCONFIG_FILE
echo '  eval "echo $o"' >>$LIBCONFIG_FILE
echo 'fi' >>$LIBCONFIG_FILE
echo ' ' >>$LIBCONFIG_FILE
echo '# help text' >>$LIBCONFIG_FILE
echo 'if test ! -z "$h" ; then ' >>$LIBCONFIG_FILE
echo '  cat <<EOF' >>$LIBCONFIG_FILE
echo 'All available options:' >>$LIBCONFIG_FILE
echo '  --prefix=DIR and   change \$prefix and \$exec-prefix' >>$LIBCONFIG_FILE
echo '  --exec-prefix=DIR  (affects all other options)' >>$LIBCONFIG_FILE
echo '  --prefix           \$prefix        $prefix' >>$LIBCONFIG_FILE
echo '  --exec_prefix  or... ' >>$LIBCONFIG_FILE
echo '  --eprefix          \$exec_prefix   $exec_prefix' >>$LIBCONFIG_FILE
echo '  --version          \$version       $version' >>$LIBCONFIG_FILE
echo '  --cflags           -I\$includedir  unless it is /usr/include' >>$LIBCONFIG_FILE
echo '  --libs             -L\$libdir \$LIBS $libs' >>$LIBCONFIG_FILE
echo '  --package          \$package       $package' >>$LIBCONFIG_FILE
echo '  --bindir           \$bindir        $bindir' >>$LIBCONFIG_FILE
echo '  --sbindir          \$sbindir       $sbindir' >>$LIBCONFIG_FILE
echo '  --libexecdir       \$libexecdir    $libexecdir' >>$LIBCONFIG_FILE
echo '  --datadir          \$datadir       $datadir' >>$LIBCONFIG_FILE
echo '  --sysconfdir       \$sysconfdir    $sysconfdir' >>$LIBCONFIG_FILE
echo '  --sharedstatedir   \$sharedstatedir$sharedstatedir' >>$LIBCONFIG_FILE
echo '  --localstatedir    \$localstatedir $localstatedir' >>$LIBCONFIG_FILE
echo '  --libdir           \$libdir        $libdir' >>$LIBCONFIG_FILE
echo '  --infodir          \$infodir       $infodir' >>$LIBCONFIG_FILE
echo '  --mandir           \$mandir        $mandir' >>$LIBCONFIG_FILE
echo '  --target           \$target        $target' >>$LIBCONFIG_FILE
echo '  --host             \$host          $host' >>$LIBCONFIG_FILE
echo '  --build            \$build         $build' >>$LIBCONFIG_FILE
echo '  --pkgdatadir       \$datadir/\$package    ${datadir}/${package}'    >>$LIBCONFIG_FILE
echo '  --pkglibdir        \$libdir/\$package     ${libdir}/${package}' >>$LIBCONFIG_FILE
echo '  --pkgincludedir    \$includedir/\$package ${includedir}/${package}' >>$LIBCONFIG_FILE
echo '  --template-version \$template_version     $template_version' >>$LIBCONFIG_FILE
echo '  --help' >>$LIBCONFIG_FILE
echo 'EOF' >>$LIBCONFIG_FILE
echo 'fi' >>$LIBCONFIG_FILE
m4_pushdef([LIBCONFIG_UP], [m4_translit([$1], [a-z], [A-Z])])dnl
LIBCONFIG_UP[]_CONFIG="LIBCONFIG_DIR[]$1-config"
AC_SUBST(LIBCONFIG_UP[]_CONFIG)
dnl AC_CONFIG_FILES(LIBCONFIG_DIR[]$1[-config], [chmod +x ]LIBCONFIG_DIR[]$1[-config])
m4_popdef([LIBCONFIG_DIR])
m4_popdef([LIBCONFIG_UP])
])


# AC_CONFIG_LIBCONFIG_IN_MODULES [(MODULE1 [, MODULE2 [, ...]])]
# --------------------------------------------------------------
#Output shell script using custom module variables.
AC_DEFUN([AC_CONFIG_LIBCONFIG_IN_MODULES],
[m4_if([$1], , ,
       [# create module definition for $1
dnl we're going to need uppercase, lowercase and user-friendly versions of the
dnl string `MODULE'
m4_pushdef([MODULE_UP], m4_translit([$1], [a-z], [A-Z]))dnl
m4_pushdef([MODULE_DOWN], m4_translit([$1], [A-Z], [a-z]))dnl
if test -z "$MODULE_DOWN[]_cflags" ; then
  if test -n "$MODULE_UP[]_CFLAGS" ; then
      MODULE_DOWN[]_cflags="$MODULE_UP[]_CFLAGS"
  else
dnl    AC_MSG_WARN([variable `MODULE_DOWN[]_cflags' undefined])
    MODULE_DOWN[]_cflags=''
  fi
fi
AC_SUBST(MODULE_DOWN[]_cflags)dnl
if test -z "$MODULE_DOWN[]_libs" ; then
  if test -n "$MODULE_UP[]_LIBS" ; then
    MODULE_DOWN[]_libs="$MODULE_UP[]_LIBS"
  else
    AC_MSG_WARN([variable `MODULE_DOWN[]_libs' and `MODULE_UP[]_LIBS' undefined])
    MODULE_DOWN[]_libs='-l$1'
  fi
  if test -n "$MODULE_UP[]_LIBDEPS" ; then
    MODULE_DOWN[]_libs="$MODULE_DOWN[]_libs $MODULE_UP[]_LIBDEPS"
  fi
fi
AC_SUBST(MODULE_DOWN[]_libs)dnl
if test -z "$MODULE_UP[]_VERSION" ; then
  AC_MSG_WARN([variable `MODULE_UP[]_VERSION' undefined])
  MODULE_UP[]_VERSION="$VERSION"
fi
AC_SUBST(MODULE_UP[]_VERSION)dnl
echo 'if test x$echo_module_$1 = xyes ; then' >>$LIBCONFIG_FILE
AC_CONFIG_LIBCONFIG_IN_MODULES_VARS([cflags], [MODULE_DOWN[]_cflags], [cflags])
AC_CONFIG_LIBCONFIG_IN_MODULES_VARS([libs], [MODULE_DOWN[]_libs], [libs])
AC_CONFIG_LIBCONFIG_IN_MODULES_VARS([version], [MODULE_UP[]_VERSION], [modversion])
echo 'fi' >>$LIBCONFIG_FILE
m4_popdef([MODULE_UP])dnl
m4_popdef([MODULE_DOWN])dnl
AC_CONFIG_LIBCONFIG_IN_MODULES(m4_shift($@))])dnl
])


# AC_CONFIG_LIBCONFIG_IN_MODULES_VARS [(VAR, SUBSTITUTION,
# PKGCONFIG_ARGS)]
# --------------------------------------------------------
# Output AC_CONFIG_LIBCONFIG_IN_MODULES variables.
#   VAR = variable to set
#   SUBSTITUTION = set if pkg-config is not available
#   PKGCONFIG_ARGS = args for pkg-config
AC_DEFUN([AC_CONFIG_LIBCONFIG_IN_MODULES_VARS],
[m4_if(AC_CONFIG_LIBCONFIG_IN_USEPKGCONFIG, [true],
[
echo 'if test -x "`which pkg-config`" ; then' >>$LIBCONFIG_FILE
echo '  if pkg-config --atleast-pkgconfig-version=0.7 --exists "MODULE_DOWN"; then' >>$LIBCONFIG_FILE
echo '    $1="@S|@$1 `pkg-config --$3 MODULE_DOWN`"' >>$LIBCONFIG_FILE
echo '  fi' >>$LIBCONFIG_FILE
echo 'else' >>$LIBCONFIG_FILE
echo '  $1="@S|@$1 @$2@"' >>$LIBCONFIG_FILE
echo 'fi' >>$LIBCONFIG_FILE
],
[echo '  $1="@S|@$1 @$2@"' >>$LIBCONFIG_FILE
])])


# AC_CONFIG_LIBCONFIG_IN_PKGCONFIG
# --------------------------------
# Enable pkgconfig support in libconfig script (default)
AC_DEFUN([AC_CONFIG_LIBCONFIG_IN_PKGCONFIG],
[m4_define([AC_CONFIG_LIBCONFIG_IN_USEPKGCONFIG], [true])
])dnl


# AC_CONFIG_LIBCONFIG_IN_STATIC
# -----------------------------
# Disable pkgconfig support in libconfig script
AC_DEFUN([AC_CONFIG_LIBCONFIG_IN_STATIC],
[m4_define([AC_CONFIG_LIBCONFIG_IN_USEPKGCONFIG], [false])
])dnl

dnl ______ /home/klmitch/devel/acinclude/ac_define_version.m4 ______
dnl AC_DEFINE_VERSION(library, header, cfile, version, prefix)
AC_DEFUN([AC_DEFINE_VERSION],
[# create files to designate the version
dnl get upper- and lower-case versions of string `LIBRARY'
m4_pushdef([UP], m4_translit([$1], [a-z], [A-Z]))dnl
m4_pushdef([DOWN], m4_translit([$1], [A-Z], [a-z]))dnl
m4_pushdef([UPREFIX], m4_if([$5], , UP, m4_translit([$5], [a-z], [A-Z])))dnl
m4_pushdef([LPREFIX], m4_if([$5], , DOWN, m4_translit([$5], [A-Z], [a-z])))dnl

UPREFIX[]_VERSION_HEADER=[]m4_if([$2], , "$1[]_version.h", [$2])
UPREFIX[]_VERSION_CFILE=[]m4_if([$3], , "$1[]_version.c", [$3])
lib_version=[]m4_if([$4], , "$UP[]_VERSION", [$4])

UPREFIX[]_MAJOR_VERSION=`echo "$lib_version" | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
UPREFIX[]_MINOR_VERSION=`echo "$lib_version" | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
UPREFIX[]_MICRO_VERSION=`echo "$lib_version" | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

AC_MSG_NOTICE([configuring $1 version: $UPREFIX[]_MAJOR_VERSION.$UPREFIX[]_MINOR_VERSION.$UPREFIX[]_MICRO_VERSION])

AC_MSG_NOTICE([creating $UPREFIX[]_VERSION_HEADER])
rm -f $UPREFIX[]_VERSION_HEADER
echo "/* $UPREFIX[]_VERSION_HEADER library version header" >> $UPREFIX[]_VERSION_HEADER
echo ' * generated by ac_define_version.m4' >> $UPREFIX[]_VERSION_HEADER
echo ' */' >> $UPREFIX[]_VERSION_HEADER
echo '' >> $UPREFIX[]_VERSION_HEADER
echo "@%:@define UPREFIX[]_MAJOR_VERSION ($UPREFIX[]_MAJOR_VERSION)" >> $UPREFIX[]_VERSION_HEADER
echo "@%:@define UPREFIX[]_MINOR_VERSION ($UPREFIX[]_MINOR_VERSION)" >> $UPREFIX[]_VERSION_HEADER
echo "@%:@define UPREFIX[]_MICRO_VERSION ($UPREFIX[]_MICRO_VERSION)" >> $UPREFIX[]_VERSION_HEADER
echo '' >> $UPREFIX[]_VERSION_HEADER
echo 'extern const unsigned int LPREFIX[]_major_version;' >> $UPREFIX[]_VERSION_HEADER
echo 'extern const unsigned int LPREFIX[]_minor_version;' >> $UPREFIX[]_VERSION_HEADER
echo 'extern const unsigned int LPREFIX[]_micro_version;' >> $UPREFIX[]_VERSION_HEADER

AC_MSG_NOTICE([creating $UPREFIX[]_VERSION_CFILE])
rm -f $UPREFIX[]_VERSION_CFILE
echo "/* $UPREFIX[]_VERSION_CFILE library C file" >> $UPREFIX[]_VERSION_CFILE
echo ' * generated by ac_define_version.m4' >> $UPREFIX[]_VERSION_CFILE
echo ' */' >> $UPREFIX[]_VERSION_CFILE
dnl echo "#include \"$UPREFIX[]_VERSION_HEADER\"" >> $UPREFIX[]_VERSION_CFILE
echo '' >> $UPREFIX[]_VERSION_CFILE
echo "const unsigned int LPREFIX[]_major_version = $UPREFIX[]_MAJOR_VERSION;" >> $UPREFIX[]_VERSION_CFILE
echo "const unsigned int LPREFIX[]_minor_version = $UPREFIX[]_MINOR_VERSION;" >> $UPREFIX[]_VERSION_CFILE
echo "const unsigned int LPREFIX[]_micro_version = $UPREFIX[]_MICRO_VERSION;" >> $UPREFIX[]_VERSION_CFILE

AC_SUBST(UPREFIX[]_MAJOR_VERSION)
AC_SUBST(UPREFIX[]_MINOR_VERSION)
AC_SUBST(UPREFIX[]_MICRO_VERSION)
AC_SUBST_FILE(UPREFIX[]_VERSION_HEADER)
AC_SUBST_FILE(UPREFIX[]_VERSION_CFILE)

dnl Pop the macros we defined earlier
m4_popdef([UP])dnl
m4_popdef([DOWN])dnl
m4_popdef([UPREFIX])dnl
m4_popdef([LPREFIX])dnl
])# AC_DEFINE_VERSION

