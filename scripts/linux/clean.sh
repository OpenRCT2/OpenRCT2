#!/bin/bash

set -ev

if [[ $TRAVIS != "true" ]]
then
	echo This script is only meant to be run on Travis-CI.
	echo Please use CMake to build the project.
	exit 1
fi


sudo rm -rf /usr/local/cross-tools/i686-w64-mingw32
sudo rm -rf /usr/local/cross-tools/orcalibs
sudo rm -rf /usr/local/cross-tools/orctlibs
rm -rf .cache
rm -rf build
