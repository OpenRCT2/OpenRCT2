#!/bin/bash

if [[ $TRAVIS != "true" ]]
then
	echo This script is only meant to be run on Travis-CI.
	echo Please use CMake to build the project.
	exit 1
fi

SDL2_PV=2.0.3
SDL2_TTF_PV=2.0.12

cachedir=.cache
if [[ $(uname -s) == "Darwin" ]]; then
	liburl=https://openrct2.website/files/orctlibs-osx.zip
else
	liburl=https://openrct2.website/files/orctlibs.zip
fi
mkdir -p "$cachedir"

# Sets default target to "ubuntu_amd64", if none specified
TARGET=${TARGET-ubuntu_amd64}
# keep in sync with version in build.sh
libversion=3
libVFile="./libversion"

function has_cmd {
	command -v "$1" >/dev/null 2>&1
}

function calculate_sha256 {
	if has_cmd "shasum"; then
		command shasum -a 256 "$1" | cut -f1 -d" "
	elif has_cmd "sha256sum"; then
		command sha256sum "$1" | cut -f1 -d" "
	else
		echo "Please install either sha256sum or shasum to continue"
		exit 1
	fi
}

function download {
	if has_cmd "curl"; then
		curl -L -o "$2" "$1"
	elif has_cmd "wget"; then
		wget -O "$2" "$1"
	else
		echo "Please install either wget or curl to continue"
		exit 1
	fi
}

function download_libs {
	if [[ ! -f $cachedir/orctlibs.zip ]]; then
		download $liburl $cachedir/orctlibs.zip;
	fi
	if [[ ! -d $cachedir/orctlibs ]]; then
		mkdir -p $cachedir/orctlibs
		pushd $cachedir/orctlibs
			unzip -uaq ../orctlibs.zip
		popd
	fi
}

function mac_os_install_mingw_32 {
	local mingw_name="mingw-w32-bin_i686-darwin"
	local mingw_tar="${mingw_name}_20130531.tar.bz2"
	local mingw_path="/usr/local/$mingw_name"

	if [[ ! -f "$cachedir/$mingw_tar" ]]; then
		download "https://downloads.sourceforge.net/project/mingw-w64/Toolchains targetting Win32/Automated Builds/$mingw_tar" "$cachedir/$mingw_tar"
	fi

	if [[ ! -d "$mingw_path" ]]; then
		echo "Extracting contents of $mingw_tar to $mingw_path"
		echo "Don't forget to add $mingw_path/bin to your PATH variable!"

		mkdir "$mingw_path"
		tar -xyf "$cachedir/$mingw_tar" -C "$mingw_path"

		pushd "$mingw_path"
			find . -type d -exec chmod 755 {} \;
		popd
	fi
}

echo "HOST = $(uname)"
echo "TARGET = $TARGET"

if [[ "$(uname)" == "Darwin" ]]; then
	if ! has_cmd "brew"; then
		echo "Homebrew is not installed, or brew is not in your \$PATH"
		echo "install instructions: http://brew.sh/"
		exit 1
	fi

	brew install cmake

	if [[ $TARGET == "windows" ]]; then
		brew install wine
		mac_os_install_mingw_32
	else
		brew install jansson sdl2 sdl2_ttf speex --universal
	fi
elif [[ $(uname) == "Linux" ]]; then
	# prevent build.sh from re-doing all the steps again
	case "$TARGET" in
		"ubuntu_i686")
			docker pull openrct2/openrct2:ubuntu_i686
			;;
		"ubuntu_amd64")
			docker pull openrct2/openrct2:ubuntu_amd64
			;;
		"windows")
			docker pull openrct2/openrct2:mingw-arch
			;;
		"docker32")
			docker pull openrct2/openrct2:32bit-only
			;;
		"docker64")
			docker pull openrct2/openrct2:64bit-only
			;;
		*)
			echo "unkown target $TARGET"
			exit 1
	esac
fi

if [[ $(uname -s) == "Darwin" ]]; then
	download_libs
	calculate_sha256 "$cachedir/orctlibs.zip" > "$libVFile"
	echo "Downloaded library with sha256sum: $(cat "$libVFile")"
	# Local libs are required for all targets
# $(uname -s) == "Darwin"
fi
