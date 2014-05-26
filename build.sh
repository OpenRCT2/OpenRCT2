#!/bin/bash

set -e

if [[ ! -d build ]]; then
	mkdir -p build
fi

pushd build
	cmake -DCMAKE_TOOLCHAIN_FILE=../CMakeLists_mingw.txt -DCMAKE_BUILD_TYPE=Debug  ..
	make
popd

if [[ -t 1 ]]; then
    echo "\033[95mDone! Run OpenRCT2 by typing:\n\nwine openrct2.exe\n\033[0m"
else
    echo "Done! Run OpenRCT2 by typing:\n\nwine openrct2.exe\n"
fi
 
