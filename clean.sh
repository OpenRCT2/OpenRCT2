#!/bin/bash

set -ev

sudo rm -rf /usr/local/cross-tools/i686-w64-mingw32
sudo rm -rf /usr/local/cross-tools/orcalibs
sudo rm -rf /usr/local/cross-tools/orctlibs
rm -rf .cache
rm -rf build
