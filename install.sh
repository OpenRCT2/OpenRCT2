#!/bin/bash

SDL2_PV=2.0.3
SDL2_TTF_PV=2.0.12

cachedir=.cache
liburl=https://openrct2.website/files/orctlibs.zip
mkdir -p $cachedir

echo $(uname)

# Sets default target to "linux", if none specified
TARGET=${TARGET-linux}
# keep in sync with version in build.sh
libversion=3
libVFile="./libversion"

function has_cmd {
	command -v "$1" >/dev/null 2>&1
}

function calculate_sha256 {
	if has_cmd "sha256sum"; then
		command sha256sum "$1" | cut -f1 -d" "
	elif has_cmd "shasum"; then
		command shasum -a 256 "$1" | cut -f1 -d" "
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

function download_sdl {
	if [[ ! -f $cachedir/SDL2-devel-${SDL2_PV}-mingw.tar.gz ]]; then
		download http://libsdl.org/release/SDL2-devel-${SDL2_PV}-mingw.tar.gz $cachedir/SDL2-devel-${SDL2_PV}-mingw.tar.gz;
	fi
	if [[ ! -f $cachedir/SDL2_ttf-devel-${SDL2_TTF_PV}-mingw.tar.gz ]]; then
		download https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${SDL2_TTF_PV}-mingw.tar.gz $cachedir/SDL2_ttf-devel-${SDL2_TTF_PV}-mingw.tar.gz;
	fi
	if [[ ! -d $cachedir/SDL2-${SDL2_PV} ]]; then
		pushd $cachedir
			tar -xzf SDL2-devel-${SDL2_PV}-mingw.tar.gz
		popd
	fi
	if [[ ! -d $cachedir/SDL2_ttf-${SDL2_TTF_PV} ]]; then
		pushd $cachedir
			tar -xzf SDL2_ttf-devel-${SDL2_TTF_PV}-mingw.tar.gz
		popd
	fi
	# Apply platform patch
	mingw_patch=libsdl2-mingw-2.0.3-fix-platform-detection-for-mingw.patch
	if [[ ! -f $cachedir/$mingw_patch ]]; then
		download "https://github.com/anyc/anyc-overlay/raw/master/media-libs/libsdl2-mingw/files/$mingw_patch" $cachedir/$mingw_patch;

		# XXX not sure how to make this idempotent.
		pushd $cachedir/SDL2-${SDL2_PV}/i686-w64-mingw32/include/SDL2/
			echo "Applying patch."
			patch -p2 < ../../../../$mingw_patch
		popd
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

function install_cross_tools {
	if [[ ! -d /usr/local/cross-tools ]]; then
		sudo mkdir -p /usr/local/cross-tools
	fi
	if [[ ! -d /usr/local/cross-tools/i686-w64-mingw32 ]]; then
		sudo cp -r $cachedir/SDL2-${SDL2_PV}/i686-w64-mingw32 /usr/local/cross-tools/
		sudo cp -r $cachedir/SDL2_ttf-${SDL2_TTF_PV}/i686-w64-mingw32 /usr/local/cross-tools/
	fi
	if [[ ! -d /usr/local/cross-tools/orctlibs ]]; then
		sudo mkdir -p /usr/local/cross-tools/orctlibs
		sudo cp -rf $cachedir/orctlibs/glob/* /usr/local/cross-tools/orctlibs/.
	fi
}

function install_pkg_config {
	if [[ ! -f $cachedir/i686-w64-mingw32-pkg-config ]]; then
		# If this fails to work because of newlines, be sure you are running this
		# script with Bash, and not sh. We should really move this to a separate
		# file.
		echo -e "#!/bin/sh\nexport PKG_CONFIG_PATH=/usr/local/cross-tools/i686-w64-mingw32/lib/pkgconfig:/usr/local/cross-tools/orctlibs/lib/pkgconfig\npkg-config \$@" > $cachedir/i686-w64-mingw32-pkg-config;
	fi

	chmod +x $cachedir/i686-w64-mingw32-pkg-config
	sudo cp $cachedir/i686-w64-mingw32-pkg-config /usr/local/bin/

	ls -al /usr/local/bin | grep pkg-config
	cat /usr/local/bin/i686-w64-mingw32-pkg-config
}

function install_local_libs {
	mkdir -p lib
	cp -rf $cachedir/orctlibs/local/* ./lib/.
}

function os_x_install_mingw_32 {
	mingw_name=mingw-w32-bin_i686-darwin
	mingw_tar=$mingw_name"_20130531".tar.bz2
	mingw_path=/usr/local/$mingw_name

	if [[ ! -f $cachedir/$mingw_tar ]]; then
		download "https://downloads.sourceforge.net/project/mingw-w64/Toolchains targetting Win32/Automated Builds/$mingw_tar" $cachedir/$mingw_tar
	fi

	if [[ ! -d "$mingw_path" ]]; then
		echo "Extracting contents of $mingw_tar to $mingw_path"
		echo "Don't forget to add $mingw_path/bin to your PATH variable!"

		mkdir $mingw_path
		tar -xyf $cachedir/$mingw_tar -C $mingw_path

		pushd $mingw_path
			find . -type d -exec chmod 755 {} \;
		popd
	fi
}

function os_x_install_bottle {
	local checksum=$1
	local url=$2
	local target="$cachedir/$(basename "$url")"

	download "$url" "$target"
	local fingerprint=$(calculate_sha256 "$target")

	if [[ "$fingerprint" != "$checksum" ]]; then
		echo "Bottle sha256 sum missmatch!"
		echo " url:      $url"
		echo " expected: $checksum"
		echo " actual:   $fingerprint"
		exit 1
	fi

	brew install "$target"
}

echo TARGET = $TARGET

if [[ $(uname) == "Darwin" ]]; then
	echo "Installation of OpenRCT2 assumes you have homebrew and use it to install packages."

	if ! has_cmd "brew"; then
		echo "brew is not installed, or is not in your \$PATH"
		echo "install instructions: http://brew.sh/"
		exit 1
	fi

	if [[ -z "$TRAVIS" ]]; then
		brew install cmake
		brew install jansson sdl2 sdl2_ttf speex --universal
	else
		os_x_install_bottle "85ccc126f06b33f211b4ec1910c68b5338ba8673aded33848faee4be1db31436" "https://www.dropbox.com/s/9anb2fiphhinzh1/jansson-2.7.el_capitan.bottle.1.tar.gz"
		os_x_install_bottle "9c3a0d420e4b4f94e9313ea8613750cccff6a4854947eab6857606c6ad56ed98" "https://www.dropbox.com/s/11mulewbqocxhbv/sdl2_ttf-2.0.12.el_capitan.bottle.tar.gz"
		os_x_install_bottle "0cc4f89cc534839b575ad8c4dffbf03bd5904e04168fcef6a7342adbab93e79e" "https://www.dropbox.com/s/bvowsics73vqnyy/sdl2-2.0.3.el_capitan.bottle.2.tar.gz"
		os_x_install_bottle "7d2817ec382334b52a577201266df037101a5c35812e887a32613c13976f07ae" "https://www.dropbox.com/s/bo6eao0uf4di6k4/speex-1.2rc1.el_capitan.bottle.3.tar.gz"
	fi

	if [[ $TARGET == "windows" ]]; then
		brew install wine
		os_x_install_mingw_32
	fi
elif [[ $(uname) == "Linux" ]]; then
	if [[ -z "$TRAVIS" ]]; then
	    sudo apt-get install -y binutils-mingw-w64-i686 gcc-mingw-w64-i686 g++-mingw-w64-i686 cmake
		if [[ -z "$DISABLE_G2_BUILD" ]]; then
			sudo apt-get install -y wine
		fi
	else
		# prevent build.sh from re-doing all the steps again
		case "$TARGET" in
			"linux")
				sudo dpkg --add-architecture i386
				sudo apt-get update
				sudo apt-get install --no-install-recommends -y --force-yes cmake libsdl2-dev:i386 libsdl2-ttf-dev:i386 gcc-4.8 pkg-config:i386 g++-4.8-multilib gcc-4.8-multilib libjansson-dev:i386 libspeex-dev:i386 libspeexdsp-dev:i386 libcurl4-openssl-dev:i386 libcrypto++-dev:i386 clang
				download https://launchpad.net/ubuntu/+archive/primary/+files/libjansson4_2.7-1ubuntu1_i386.deb libjansson4_2.7-1ubuntu1_i386.deb
				download https://launchpad.net/ubuntu/+archive/primary/+files/libjansson-dev_2.7-1ubuntu1_i386.deb libjansson-dev_2.7-1ubuntu1_i386.deb
				sudo dpkg -i libjansson4_2.7-1ubuntu1_i386.deb
				sudo dpkg -i libjansson-dev_2.7-1ubuntu1_i386.deb
				sudo apt-get install -f
				;;
			"windows")
				sudo apt-get update
				sudo apt-get install -y --force-yes binutils-mingw-w64-i686 gcc-mingw-w64-i686 g++-mingw-w64-i686 cmake
				;;
			"docker32")
				docker pull openrct2/openrct2:32bit-only
				;;
			*)
				echo "unkown target $TARGET"
				exit 1
		esac
	fi
fi

download_libs
# mind the gap (trailing space)
calculate_sha256 $cachedir/orctlibs.zip > $libVFile
echo "Downloaded library with sha256sum: $(cat $libVFile)"
# Local libs are required for all targets
install_local_libs

if [[ $TARGET == "windows" ]]; then
	download_sdl
	install_cross_tools
	install_pkg_config
# $TARGET == "windows"
fi
