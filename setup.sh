export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/lib
if hash lsetup 2>/dev/null; then
    lsetup root
fi
GCCVERSION=$(gcc --version | grep ^gcc | sed 's/^.* //g')
GCC_GTEQ_47=$(expr `gcc -dumpversion | sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e '\
s/^[0-9]\{3,4\}$$/&00/'` \>= 40700)
if [ "$GCC_GTEQ_47" -ne "1" ]; then
    echo -e "\\033[38;5;196mWarning: Your GCC version ($GCCVERSION) does not support C++11 features, \
which are used in AnalysisTools.\\033[0m"
fi