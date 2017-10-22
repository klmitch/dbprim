#! /bin/sh

aclocal --install -I m4
autoconf
libtoolize --automake --copy
automake -a -c
