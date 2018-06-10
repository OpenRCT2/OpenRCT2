FROM ubuntu:18.04

# Ubuntu 18.04 already comes with GCC 7 available, there's no need to install PPA for it,
# but leave the commands for future updates

#RUN \
#  apt-get update && \
#  apt-get install -y software-properties-common && \
#  add-apt-repository ppa:ubuntu-toolchain-r/test && \
#  apt-get update && \
#  apt-get install -y g++-7 && \
#  update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 60

# Ubuntu 18.04 already comes with clang 5.0 available, there's no need to install repository for it,
# but leave the commands for future updates
#RUN apt-get update \
#	&& apt-get install -y curl sudo \
#	&& curl http://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add - \
#	&& echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-5.0 main" > /etc/apt/sources.list.d/clang5.list \
#	&& apt-get update \
#	&& apt-get install -y \
#	clang-5.0 \
#	git-core \
#	&& ln -s /usr/bin/clang-5.0 /usr/bin/clang \
#	&& ln -s /usr/bin/clang++-5.0 /usr/bin/clang++

# Ubuntu 18.04 already comes with cmake 3.10 available, there's no need to install it manually,
# but leave the commands for future updates
#RUN curl https://cmake.org/files/v3.9/cmake-3.9.4-Linux-x86_64.sh -o cmake.sh && sh cmake.sh --skip-license --exclude-subdir --prefix=/usr/local

RUN \
  apt-get update && \
  apt-get install -y git-core g++ clang-5.0 && \
  ln -s /usr/bin/clang-5.0 /usr/bin/clang && \
  ln -s /usr/bin/clang++-5.0 /usr/bin/clang++

RUN apt-get -y upgrade
# clang and gcc already installed
RUN apt-get install --no-install-recommends -y ccache cmake libsdl2-dev libsdl2-ttf-dev pkg-config libjansson-dev libspeex-dev libspeexdsp-dev libcurl4-openssl-dev libcrypto++-dev libfontconfig1-dev libfreetype6-dev libpng-dev libzip-dev git libssl-dev ninja-build libicu-dev
