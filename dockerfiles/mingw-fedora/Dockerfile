FROM fedora:30

# Update base system
RUN dnf update -y

# Add some useful packages to the base system
RUN dnf install -y \
        make \
        cmake \
        ninja-build

# Install MingW packages
RUN dnf install -y \
        mingw32-binutils \
        mingw32-boost \
        mingw32-crt \
        mingw32-curl \
        mingw32-fontconfig \
        mingw32-freetype \
        mingw32-gcc \
        mingw32-headers \
        mingw32-jansson \
        mingw32-libpng \
        mingw32-libzip \
        mingw32-openssl \
        mingw32-pkg-config \
        mingw32-SDL2 \
        mingw32-SDL2_ttf \
        mingw32-speexdsp \
        mingw-w64-tools \
        mingw32-winpthreads \
        mingw32-zlib

RUN useradd --create-home --comment "Fedora Build User" build

USER build

WORKDIR /tmp

# Overrides from fedora-base
ENV HOME /home/build
