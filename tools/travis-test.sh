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
${build_root}/configure --enable-code-coverage

# Run a check
banner 1 "  Running \"make check\""
make check

# Submit the code coverage report
banner 1 "  Submitting code coverage report"
coveralls --verbose --dump coverage.json --exclude tests --root ${build_root} --build-root ${build_root}

# Run a make distcheck
banner 1 "  Running \"make distcheck\""
make distcheck

# Run a documentation build
banner 1 "  Running \"make doxygen-doc\""
make doxygen-doc

# Done; go back to the build root
banner 1 "Done testing dbprim library"
