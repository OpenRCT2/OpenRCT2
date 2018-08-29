#! /bin/bash

set -e
set -x

# use RAM disk if possible
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" OpenRCT2-appimage-build-XXXXXX)

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

# standard linuxdeploy pattern
#see https://docs.appimage.org/packaging-guide/from-source/index.html for more information
cmake "$REPO_ROOT" -DCMAKE_INSTALL_PREFIX=/usr

make -j$(nproc) VERBOSE=1
make install DESTDIR=AppDir

wget https://github.com/TheAssassin/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

./linuxdeploy*.AppImage --appimage-extract-and-run --appdir AppDir/ --output appimage

mv OpenRCT2*.AppImage "$OLD_CWD"/
