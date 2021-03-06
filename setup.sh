#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/lib
if hash lsetup 2>/dev/null; then
    lsetup "root 6.08.02-x86_64-slc6-gcc49-opt"
fi

UNAME=`uname`
COMPILER="gcc"
if [[ "$UNAME" == "Darwin"* ]]; then
    if hash clang 2>/dev/null; then
        COMPILER="clang"
    fi
fi

if [[ "$COMPILER" == "gcc" ]]; then
    GCCVERSION=$(gcc --version | grep ^gcc | sed 's/^.* //g')
    GCC_GTEQ_47=$(expr `gcc -dumpversion | sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e '\
s/^[0-9]\{3,4\}$$/&00/'` \>= 40700)
    if [ "$GCC_GTEQ_47" -ne "1" ]; then
        echo -e "\\033[38;5;196mWarning: Your GCC version ($GCCVERSION) does not support C++11 features, \
which are used in AnalysisTools.\\033[0m"
    fi
fi

# Perform check for clang?