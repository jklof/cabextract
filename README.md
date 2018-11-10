# cabextract


A C++ library that permits access to CAB files.

HOW TO BUILD
------------

get depencies (gtest and libmspack)

    ./build_deps.sh

build lib, test program and googletests

    make all

run googletests

    ./cabextract_gtest

run extract / inspection tool list files in a cab archive

    ./cabextract -l <cab>

 extract a file from cab archive
 
    ./cabextract -e <cab> <cabfile> <destfile>

There is one example file in the testdata directory.





