#!/bin/bash
#fetch our extra depencies

#if [ ! -d libmspack ]; then
#    git clone https://github.com/kyz/libmspack.git
#    pushd libmspack/libmspack
#    ./autogen.sh
#    ./configure
#    make
#    popd
#fi

dpkg -l libmspack-dev >/dev/null

if [ $? -ne 0 ] ; then
    echo "libmspack-dev seems to be missing, trying to install it"
    sudo apt-get install libmspack-dev
fi

if [ ! -d googletest ]; then
    git clone https://github.com/google/googletest.git
    pushd googletest
    cmake .
    make
    popd
fi
