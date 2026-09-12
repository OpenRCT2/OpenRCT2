#!/bin/sh
# Cross-build OpenRCT2's mandatory C dependencies for m68k-amigaos as static libraries.
#   usage: build-deps.sh <install-prefix> [amiga-gcc-prefix]     (default ~/opt/amiga16)
# Needs: cmake, make, curl, tar. Produces <install-prefix>/{lib,include,lib/pkgconfig}.
set -e
INST="${1:?install prefix required}"; AMIGA="${2:-$HOME/opt/amiga16}"
HERE="$(cd "$(dirname "$0")" && pwd)"; TC="$HERE/../../cmake/toolchains/m68k-amigaos.cmake"
CPU="${AMIGA_CPU_FLAGS:--m68020 -msoft-float}"
WORK="${AMIGA_DEPS_WORK:-$INST-build}"; mkdir -p "$WORK" "$INST/lib/pkgconfig" "$INST/include"
export PATH="$AMIGA/bin:$PATH"
export CC=m68k-amigaos-gcc CXX=m68k-amigaos-g++ AR=m68k-amigaos-ar RANLIB=m68k-amigaos-ranlib
export CFLAGS="-O2 $CPU -fsigned-char -mcrt=nix20 -fomit-frame-pointer" CXXFLAGS="-O2 $CPU -fsigned-char -mcrt=nix20 -fomit-frame-pointer" LDFLAGS="-mcrt=nix20"
fetch() { [ -f "$WORK/$2" ] || curl -sSL -o "$WORK/$2" "$1"; }
step() { printf '\n=== %s ===\n' "$*"; }

step zlib 1.3.1
fetch https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz zlib-1.3.1.tar.gz
cd "$WORK"; rm -rf zlib-1.3.1; tar xzf zlib-1.3.1.tar.gz; mkdir zlib-1.3.1/b; cd zlib-1.3.1/b
# zlib's configure script reaches for Apple libtool on Darwin; its CMake build cross-compiles cleanly.
cmake .. -DCMAKE_TOOLCHAIN_FILE="$TC" -DAMIGA_PREFIX="$AMIGA" -DAMIGA_CPU_FLAGS="$CPU" -DCMAKE_INSTALL_PREFIX="$INST" -DCMAKE_BUILD_TYPE=Release -DZLIB_BUILD_EXAMPLES=OFF >/dev/null
make -j8 zlibstatic >/dev/null && cp libzlibstatic.a "$INST/lib/libz.a" && cp ../zlib.h zconf.h "$INST/include/" && cp zlib.pc "$INST/lib/pkgconfig/"

step zstd 1.5.7
fetch https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz zstd-1.5.7.tar.gz
cd "$WORK"; rm -rf zstd-1.5.7; tar xzf zstd-1.5.7.tar.gz; cd zstd-1.5.7/lib
make -j8 libzstd.a ZSTD_LEGACY_SUPPORT=0 ZSTD_LIB_DEPRECATED=0 >/dev/null
cp libzstd.a "$INST/lib/"; cp zstd.h zdict.h zstd_errors.h "$INST/include/"
printf 'prefix=%s\nexec_prefix=${prefix}\nlibdir=${prefix}/lib\nincludedir=${prefix}/include\n\nName: zstd\nDescription: zstd\nVersion: 1.5.7\nLibs: -L${libdir} -lzstd\nCflags: -I${includedir}\n' "$INST" > "$INST/lib/pkgconfig/libzstd.pc"

step libpng 1.6.50
fetch https://download.sourceforge.net/libpng/libpng-1.6.50.tar.gz libpng-1.6.50.tar.gz
cd "$WORK"; rm -rf libpng-1.6.50; tar xzf libpng-1.6.50.tar.gz; mkdir libpng-1.6.50/b; cd libpng-1.6.50/b
cmake .. -DCMAKE_TOOLCHAIN_FILE="$TC" -DAMIGA_PREFIX="$AMIGA" -DAMIGA_CPU_FLAGS="$CPU" -DCMAKE_INSTALL_PREFIX="$INST" -DCMAKE_BUILD_TYPE=Release \
  -DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_TESTS=OFF -DPNG_TOOLS=OFF -DPNG_FRAMEWORK=OFF -DPNG_HARDWARE_OPTIMIZATIONS=OFF \
  -DZLIB_ROOT="$INST" -DZLIB_INCLUDE_DIR="$INST/include" -DZLIB_LIBRARY="$INST/lib/libz.a" >/dev/null
make -j8 >/dev/null && make install >/dev/null

step libzip 1.11.4
fetch https://github.com/nih-at/libzip/releases/download/v1.11.4/libzip-1.11.4.tar.gz libzip-1.11.4.tar.gz
cd "$WORK"; rm -rf libzip-1.11.4; tar xzf libzip-1.11.4.tar.gz; mkdir libzip-1.11.4/b; cd libzip-1.11.4/b
cmake .. -DCMAKE_TOOLCHAIN_FILE="$TC" -DAMIGA_PREFIX="$AMIGA" -DAMIGA_CPU_FLAGS="$CPU" -DCMAKE_INSTALL_PREFIX="$INST" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
  -DENABLE_BZIP2=OFF -DENABLE_LZMA=OFF -DENABLE_ZSTD=ON -DENABLE_OPENSSL=OFF -DENABLE_GNUTLS=OFF -DENABLE_MBEDTLS=OFF -DENABLE_COMMONCRYPTO=OFF -DENABLE_WINDOWS_CRYPTO=OFF \
  -DHAVE_FSEEKO=0 -DHAVE_FTELLO=0 -DHAVE_MEMCPY_S=0 -DHAVE_STRNCPY_S=0 -DHAVE_STRERRORLEN_S=0 -DHAVE_STRERROR_S=0 -DHAVE_STRCASECMP=1 \
  -DHAVE_CLONEFILE=0 -DHAVE_EXPLICIT_BZERO=0 -DHAVE_EXPLICIT_MEMSET=0 -DHAVE_FCHMOD=0 -DHAVE_GETPROGNAME=0 -DHAVE_GETSECURITYINFO=0 \
  -DHAVE_LOCALTIME_R=0 -DHAVE_RANDOM=0 -DHAVE_SETMODE=0 -DHAVE_STRICMP=0 -DHAVE__CLOSE=0 -DHAVE__DUP=0 -DHAVE__FDOPEN=0 \
  -DHAVE__FILENO=0 -DHAVE__SETMODE=0 -DHAVE__STRDUP=0 -DHAVE__UNLINK=0 -DHAVE_ARC4RANDOM=0 -DHAVE_CRYPTO=0 -DHAVE_FICLONERANGE=0 -DHAVE_FTS_H=0 \
  -DBUILD_TOOLS=OFF -DBUILD_REGRESS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_DOC=OFF -DBUILD_OSSFUZZ=OFF \
  -DZLIB_ROOT="$INST" -DZLIB_INCLUDE_DIR="$INST/include" -DZLIB_LIBRARY="$INST/lib/libz.a" \
  -DZstd_INCLUDE_DIR="$INST/include" -DZstd_LIBRARY="$INST/lib/libzstd.a" >/dev/null
make -j8 >/dev/null && make install >/dev/null
echo; echo "installed into $INST:"; ls "$INST/lib"
