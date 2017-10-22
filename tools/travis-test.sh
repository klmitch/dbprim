#!/bin/sh

# Exit on the first error
set -e

# Determine the build tree directory
if [ "${CI}" = true ]; then
    build_root="${TRAVIS_BUILD_DIR}"
else
    build_root=`dirname $0`/..
fi

# Canonicalize the directory and switch there
cd ${build_root}
build_root=`pwd`

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

# Begin by generating the autoconf files
banner 0 "  Running autogen.sh..."
./autogen.sh >/dev/null 2>&1

# Create a build directory and cd there
banner 0 "  Creating build directory"
if [ ! -d travis-build ]; then
    mkdir travis-build
fi
cd travis-build

# Configure the source
banner 0 "  Configuring the source"
${build_root}/configure
banner 1 "Contents of Makefile"
cat Makefile
# Run a simple make distcheck
banner 1 "  Running \"make distcheck\""
make distcheck

# Done; go back to the build root
banner 1 "Done testing dbprim library"
