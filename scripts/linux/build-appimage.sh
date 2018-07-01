#! /bin/bash

set -e
set -x

# use RAM disk if possible
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" AppImageUpdate-build-XXXXXX)

cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}

[ "$NO_CLEANUP" == "" ] && trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname "$0")/../..)
OLD_CWD=$(readlink -f .)

pushd "$BUILD_DIR"

#wget https://raw.githubusercontent.com/wheybags/glibc_version_header/master/version_headers/force_link_glibc_2.27.h

#flags="-include "$(readlink -f "force_link_glibc_2.27.h")
CC=clang CXX=clang++ CFLAGS="$flags" CXXFLAGS="$flags" cmake "$REPO_ROOT" -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_EXE_LINKER_FLAGS="-pthread"

make -j$(nproc) VERBOSE=1
make install DESTDIR=AppDir

wget https://github.com/TheAssassin/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy*.AppImage
./linuxdeploy*.AppImage --appimage-extract
wget https://github.com/TheAssassin/linuxdeploy-plugin-checkrt/releases/download/continuous/linuxdeploy-plugin-checkrt-x86_64.sh
chmod +x linuxdeploy-plugin-checkrt-*.sh
mv linuxdeploy-plugin-checkrt-*.sh squashfs-root/usr/bin
ARCH=x86_64 squashfs-root/AppRun --appdir AppDir/ --output appimage -l /lib/x86_64-linux-gnu/libz.so.1

mv OpenRCT2*.AppImage "$OLD_CWD"/
