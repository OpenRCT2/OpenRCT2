#!/bin/bash

set -ev

sudo rm -rf /usr/local/cross-tools/i686-w64-mingw32
#rm -rf .cache
rm -rf .cache/*.patch
rm -rf .cache/SDL2-2.0.3
rm -rf .cache/i686-w64-mingw32-pkg-config
rm -rf build
