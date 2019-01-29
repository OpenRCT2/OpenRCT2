#!/bin/bash
set -e

if [[ $TRAVIS == "true" ]]; then
    echo "Setting up CI env"
    CCACHE_DIR=.cache
    mkdir -p $CCACHE_DIR
fi

mkdir -p build
pushd build
	ccache --show-stats > ccache_before
	cmake ../ -G Ninja $OPENRCT2_CMAKE_OPTS
	ninja openrct2-app -j 4 $OPENRCT_MAKE_OPTS
	ccache --show-stats > ccache_after
	diff -U100 ccache_before ccache_after
	./openrct2-cli scan-objects
	if [[ $TRAVIS == "true" ]]; then
	    cd .. && bash <(curl -s https://codecov.io/bash) 2>\&1
    fi
popd
