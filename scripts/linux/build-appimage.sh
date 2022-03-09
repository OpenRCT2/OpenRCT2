#!/bin/bash -ex

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

if [ "$NO_CLEANUP" == "" ] && trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname "$0")/../..)
OLD_CWD=$(readlink -f .)

pushd "$BUILD_DIR"

export VERSION=$(cd "$REPO_ROOT" && git describe --tags)

# standard linuxdeploy pattern
#see https://docs.appimage.org/packaging-guide/from-source/index.html for more information
cmake "$REPO_ROOT" -DCMAKE_INSTALL_PREFIX=/usr -G Ninja -DCMAKE_BUILD_TYPE=Release

ninja -k0
DESTDIR=AppDir ninja install

curl -fLO https://github.com/TheAssassin/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

./linuxdeploy*.AppImage --appimage-extract-and-run --appdir AppDir/ --output appimage

mv OpenRCT2*.AppImage "$OLD_CWD"/
