FROM ubuntu:16.04
RUN apt-get update
RUN apt-get install -y binutils-mingw-w64-i686 gcc-mingw-w64-i686 g++-mingw-w64-i686 cmake pkg-config ninja-build
RUN echo "#!/bin/sh\nexport PKG_CONFIG_PATH=/usr/local/cross-tools/i686-w64-mingw32/lib/pkgconfig:/usr/local/cross-tools/orctlibs/lib/pkgconfig\npkg-config \$@" > /usr/local/bin/i686-w64-mingw32-pkg-config && chmod a+x /usr/local/bin/i686-w64-mingw32-pkg-config
