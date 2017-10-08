FROM ubuntu:16.04
# based on sstrakh/xenial-boost-clang but imported as source,
# due to upstream image being obsolete and we don't need boost.
RUN apt-get update \
	&& apt-get install -y curl sudo \
	&& curl http://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add - \
	&& echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-4.0 main" > /etc/apt/sources.list.d/clang4.list \
	&& apt-get update \
	&& apt-get install -y \
	clang-4.0 \
	gcc \
	g++ \
	git-core \
	&& ln -s /usr/bin/clang-4.0 /usr/bin/clang \
	&& ln -s /usr/bin/clang++-4.0 /usr/bin/clang++
RUN curl https://cmake.org/files/v3.9/cmake-3.9.4-Linux-x86_64.sh -o cmake.sh && sh cmake.sh --skip-license --exclude-subdir --prefix=/usr/local
RUN apt-get -y upgrade
# clang and gcc already installed
RUN apt-get install --no-install-recommends -y cmake libsdl2-dev libsdl2-ttf-dev pkg-config libjansson-dev libspeex-dev libspeexdsp-dev libcurl4-openssl-dev libcrypto++-dev libfontconfig1-dev libfreetype6-dev libpng-dev libzip-dev git libssl-dev ninja-build
