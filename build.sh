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
libversion=3
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

if [ $currentversion -ge $libversion ]; then
    needsdownload="false"
fi

if [ ! -d $libdir ]; then
    needsdownload="true"
fi

if [[ "$needsdownload" = "true" ]]; then
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
	echo $libversion > $libVFile
fi

pushd build
	echo OPENRCT2_CMAKE_OPTS = $OPENRCT2_CMAKE_OPTS
	if [[ $TARGET == "docker32" ]]
	then
		PARENT=`readlink -f ../`
		chmod a+rwx `pwd`
		chmod g+s `pwd`
		docker run -u travis -v $PARENT:/work/openrct2 -w /work/openrct2/build -i -t janisozaur/openrct2:32bit-only bash -c "cmake ../ $OPENRCT2_CMAKE_OPTS && make"
	else
		cmake -DCMAKE_BUILD_TYPE=Debug $OPENRCT2_CMAKE_OPTS ..
		make
	fi
popd

if [[ ! -h openrct2.dll ]]; then
    ln -s build/openrct2.dll openrct2.dll
fi

if [[ -z "$DISABLE_G2_BUILD" ]]; then
    echo Building: data/g2.dat
    ./build_g2.sh > /dev/null 2>&1
fi

if [[ -t 1 ]]; then
    echo -e "\nDone! Run OpenRCT2 by typing:\n\n\033[95mwine openrct2.exe\n\033[0m"
else
    echo -e "\nDone! Run OpenRCT2 by typing:\n\nwine openrct2.exe\n"
fi
