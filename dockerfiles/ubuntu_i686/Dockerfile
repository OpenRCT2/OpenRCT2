FROM ubuntu:16.04
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get -y upgrade
RUN apt-get install --no-install-recommends -y gcc-multilib g++-multilib cmake libsdl2-dev:i386 libsdl2-ttf-dev:i386  pkg-config:i386  libjansson-dev:i386 libspeex-dev:i386 libspeexdsp-dev:i386 libcurl4-openssl-dev:i386 libcrypto++-dev:i386 libfontconfig1-dev:i386 libfreetype6-dev:i386 libpng-dev:i386 libzip-dev:i386 libssl-dev:i386 ninja-build
