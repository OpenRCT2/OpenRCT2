#!/bin/bash

set -e

if [[ ! -d build ]]; then
	mkdir -p build
fi

pushd build
	cmake -DCMAKE_TOOLCHAIN_FILE=../CMakeLists_mingw.txt -DCMAKE_BUILD_TYPE=Debug  ..
	make
popd

if [[ ! -h openrct2.dll ]]; then 
    ln -s build/openrct2.dll openrct2.dll
fi

if [[ -t 1 ]]; then
    echo "Done! Run OpenRCT2 by typing:"
    echo 
    echo "\033[95mwine openrct2.exe"
    echo "\033[0m"
else
    echo "Done! Run OpenRCT2 by typing:"
    echo 
    echo "wine openrct2.exe"
fi
 
