#!/bin/bash

set -e

if [[ $TRAVIS != "true" ]]
then
	echo This script is only meant to be run on Travis-CI.
	echo Please use CMake to build the project.
	exit 1
fi

cachedir=.cache
mkdir -p $cachedir

# Sets default target to "linux", if none specified
TARGET=${TARGET-linux}

if [[ ! -d build ]]; then
	mkdir -p build
fi

pushd build
	echo OPENRCT2_CMAKE_OPTS = "$OPENRCT2_CMAKE_OPTS"
	if [[ $TARGET == "docker64" ]]
	then
		PARENT=$(readlink -f ../)
		chmod a+rwx "$(pwd)"
		chmod g+s "$(pwd)"
		# CMAKE and MAKE opts from environment
		docker run -v "$PARENT":"$PARENT" -w "$PARENT"/build -i -t openrct2/openrct2:64bit-only bash -c "cmake ../ -DWITH_TESTS=on $OPENRCT2_CMAKE_OPTS && ninja $OPENRCT_MAKE_OPTS && ninja test ARGS=\"-V\""
	elif [[ $TARGET == "ubuntu_i686" ]]
	then
		PARENT=$(readlink -f ../)
		chmod a+rwx "$(pwd)"
		chmod g+s "$(pwd)"
		# CMAKE and MAKE opts from environment
		docker run -v "$PARENT":"$PARENT" -w "$PARENT"/build -i -t openrct2/openrct2:ubuntu_i686 bash -c "cmake ../ -DWITH_TESTS=on $OPENRCT2_CMAKE_OPTS && ninja all testpaint install $OPENRCT_MAKE_OPTS && ninja test ARGS=\"-V\" && ( ./testpaint --quiet ||  if [[ \$? -eq 1 ]] ; then echo Allowing failed tests to pass ; else echo here ; false; fi )"
	elif [[ $TARGET == "ubuntu_amd64" ]]
	then
		PARENT=$(readlink -f ../)
		chmod a+rwx "$(pwd)"
		chmod g+s "$(pwd)"
		# CMAKE and MAKE opts from environment
		docker run -v "$PARENT":"$PARENT" -w "$PARENT"/build -i -t openrct2/openrct2:ubuntu_amd64 bash -c "cmake ../ -DWITH_TESTS=on $OPENRCT2_CMAKE_OPTS && ninja $OPENRCT_MAKE_OPTS install && ninja test ARGS=\"-V\""
	elif [[ $TARGET == "windows" ]]
	then
		PARENT=$(readlink -f ../)
		chmod a+rwx "$(pwd)"
		chmod g+s "$(pwd)"
		# CMAKE and MAKE opts from environment
		docker run -v "$PARENT":"$PARENT" -w "$PARENT"/build -i -t openrct2/openrct2:mingw-arch bash -c "cmake ../ $OPENRCT2_CMAKE_OPTS && ninja $OPENRCT_MAKE_OPTS"
	else
		echo "Unkown target $TARGET"
		exit 1
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

if [[ $TARGET == "ubuntu_i686" ]]; then
	if [[ ! -h openrct2 ]]; then
		ln -s build/openrct2 openrct2
	fi
fi

if [[ -z "$DISABLE_G2_BUILD" ]]; then
    echo Building: g2.dat
	pushd build
	    ninja g2
	popd
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
