#! /bin/sh

aclocal
autoconf
libtoolize --automake --copy
automake -a -c
