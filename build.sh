#!/bin/bash

set -e

cachedir=.cache
mkdir -p $cachedir

# Sets default target to "linux", if none specified
TARGET=${TARGET-linux}

if [[ ! -d build ]]; then
	mkdir -p build
fi

# keep in sync with version in install.sh
sha256sum=69ff98c9544838fb16384bc78af9dc1c452b9d01d919e43f5fec686d02c9bdd8
libVFile="./libversion"
libdir="./lib"
currentversion=0
needsdownload="true"

if [ -f $libVFile ]; then
    while read line; do
        currentversion=$line
        continue
    done < $libVFile
fi

if [ "z$currentversion" == "z$sha256sum" ]; then
    needsdownload="false"
fi

if [ ! -d $libdir ]; then
    needsdownload="true"
fi

if [[ "$needsdownload" = "true" ]]; then
	echo "Found library had sha256sum $currentversion, expected $sha256sum"
	echo "New libraries need to be downloaded. Clearing cache and calling ./install.sh"
	rm -rf ./lib
	if [[ -f $cachedir/orctlibs.zip ]]; then
		rm -rf $cachedir/orctlibs.zip
	fi
	if [[ -d /usr/local/cross-tools/orctlibs ]]; then
		sudo rm -rf /usr/local/cross-tools/orctlibs
	fi
	if [[ -d $cachedir/orctlibs ]]; then
		rm -rf $cachedir/orctlibs
	fi
	./install.sh
fi

pushd build
	echo OPENRCT2_CMAKE_OPTS = $OPENRCT2_CMAKE_OPTS
	if [[ $TARGET == "docker32" ]]
	then
		PARENT=$(readlink -f ../)
		chmod a+rwx $(pwd)
		chmod g+s $(pwd)
		docker run -u travis -v $PARENT:/work/openrct2 -w /work/openrct2/build -i -t openrct2/openrct2:32bit-only bash -c "cmake ../ $OPENRCT2_CMAKE_OPTS && make"
	else
		cmake -DCMAKE_BUILD_TYPE=Debug $OPENRCT2_CMAKE_OPTS ..
		make
	fi
popd

if [[ $TARGET == "windows" ]]; then
	if [[ ! -h openrct2.dll ]]; then
		ln -s build/openrct2.dll openrct2.dll
	fi
fi

if [[ ! -h build/data ]]; then
	ln -s ../data build/data
fi

if [[ $TARGET == "linux" ]] || [[ $TARGET == "docker32" ]]; then
	if [[ ! -h openrct2 ]]; then
		ln -s build/openrct2 openrct2
	fi
fi

if [[ -z "$DISABLE_G2_BUILD" ]]; then
    echo Building: data/g2.dat
    ./build_g2.sh > /dev/null 2>&1
fi

if [[ $TARGET == "windows" ]]; then
	if [[ -t 1 ]]; then
		echo -e "\nDone! Run OpenRCT2 by typing:\n\n\033[95mwine openrct2.exe\n\033[0m"
	else
		echo -e "\nDone! Run OpenRCT2 by typing:\n\nwine openrct2.exe\n"
	fi
else
	if [[ -t 1 ]]; then
		echo -e "\nDone! Run OpenRCT2 by typing:\n\n\033[95m./openrct2\n\033[0m"
	else
		echo -e "\nDone! Run OpenRCT2 by typing:\n\n./openrct2\n"
	fi
fi
