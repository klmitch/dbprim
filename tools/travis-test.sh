#!/bin/sh

# Exit on the first error
set -e

# Location of cmocka tarball
cmocka_url=https://cmocka.org/files/1.1/cmocka-1.1.1.tar.xz

# Determine the build tree directory
if [ "${CI}" = true ]; then
    build_root="${TRAVIS_BUILD_DIR}"
    coveralls_dryrun=
else
    build_root=`dirname $0`/..
    coveralls_dryrun=--dryrun
fi

# Canonicalize the directory and switch there
cd ${build_root}
build_root=`pwd`

maybe_mkdir () {
    dir_name=$1
    if [ ! -d ${dir_name} ]; then
	mkdir ${dir_name}
    fi
}

# Set up the local install tree
local=${build_root}/local
maybe_mkdir ${local}
sources=${local}/sources
maybe_mkdir ${sources}

# Set CFLAGS and LDFLAGS to reference the local install tree
export CFLAGS=-I${local}/include
export LDFLAGS=-L${local}/lib

# Add the directory to LD_LIBRARY_PATH, too
if [ -z "${LD_LIBRARY_PATH}" ]; then
    export LD_LIBRARY_PATH=${local}/lib
else
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${local}/lib
fi

# Now set up the build directory
build=${build_root}/travis-build
maybe_mkdir ${build}

# Set up the banner printer
sep="======================================================================"
banner () {
    leader=$1
    msg=$2
    if [ ${leader} -gt 0 ]; then
	echo ${sep}
    fi
    echo "${msg}"
    echo ${sep}
}

banner 1 "Running tests on ${build_root}"

# Need to get a modern version of cmocka
banner 0 "  Downloading and installing cmocka..."
cd ${sources}
cmocka_base=`basename ${cmocka_url} .tar.xz`
wget ${cmocka_url}
xzcat ${cmocka_base}.tar.xz | tar xf -
cmocka_build=${cmocka_base}/build
mkdir ${cmocka_build}
cd ${cmocka_build}
cmake -DCMAKE_INSTALL_PREFIX=${local} ..
make
make install
cd ${build_root}

# Begin by generating the autoconf files
banner 1 "  Running autogen.sh..."
./autogen.sh

# Change to the build directory
cd ${build}

# Configure the source
banner 0 "  Configuring the source"
${build_root}/configure --enable-code-coverage CFLAGS="${CFLAGS} -Wall -Werror"

# Run a check
banner 1 "  Running \"make check\""
make check || (banner 1 "  Displaying test log"; cat test-suite.log; exit 1)

# Submit the code coverage report
banner 1 "  Submitting code coverage report"
coveralls ${coveralls_dryrun} --exclude local --exclude tests \
	  --root ${build_root} --build-root ${build_root}

# Run a make distcheck
banner 1 "  Running \"make distcheck\""
make distcheck

# Run a documentation build
banner 1 "  Running \"make doxygen-doc\""
make doxygen-doc

# Done; go back to the build root
banner 1 "Done testing dbprim library"
