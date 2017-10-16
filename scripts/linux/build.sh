#!/bin/bash

set -eu

if [[ "${CONFIG:-}" = "" ]]; then
	CONFIG=${1:-}
fi

case "${CONFIG}" in
help)
	echo "Pass a configuration to build"
	exit 0
	;;
LinuxClang|Linux64Bit)
	TAG=ubuntu_amd64
	;;
Linux32Bit)
	TAG=ubuntu_i686
	;;
LinuxMinGW)
	TAG=mingw-arch
	;;
LinuxArch*)
	TAG=64bit-only
	;;
*)
	echo "Unknown configuration: ${CONFIG}"
	exit 1
	;;
esac

if [[ ${TRAVIS:-} != "true" ]]; then
	mkdir -p .cache
fi

if [[ ! -d build ]]; then
	mkdir -p build
fi

pushd build
	echo "Building configuration ${CONFIG} in openrct2/openrct2:${TAG}"
	PARENT=$(readlink -f ../)
	chmod a+rwx "$(pwd)"
	chmod g+s "$(pwd)"
	docker run \
		-v "${PARENT}:/host/source" \
		-w "${PARENT}/build" \
		-it \
		"openrct2/openrct2:${TAG}" \
		cmake \
			-D CMAKE_CONFIG=/host/source/cmake/configs/${CONFIG}.cmake \
			-D SOURCE_DIR=/host/source \
			-D BUILD_DIR=/host/source/build \
			-D DOCKER_TAG=${TAG} \
			-P /host/source/docker-build.cmake
popd
