#!/bin/sh
# Configure OpenRCT2 for m68k-amigaos.
#   usage: configure.sh <build-dir> <deps-prefix> [amiga-gcc-prefix]   (default ~/opt/amiga16)
# Then:  make -C <build-dir> openrct2 openrct2-cli
set -e
BD="${1:?build dir required}"; DEPS="${2:?deps prefix required (see build-deps.sh)}"; AMIGA="${3:-$HOME/opt/amiga16}"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
export PKG_CONFIG_LIBDIR="$DEPS/lib/pkgconfig" PKG_CONFIG_PATH=""
cmake -S "$ROOT" -B "$BD" \
  -DCMAKE_TOOLCHAIN_FILE="$ROOT/cmake/toolchains/m68k-amigaos.cmake" -DAMIGA_PREFIX="$AMIGA" -DAMIGA_DEPS="$DEPS" \
  -DAMIGA_CPU_FLAGS="${AMIGA_CPU_FLAGS:--m68020 -msoft-float}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$DEPS" \
  -DDISABLE_NETWORK=ON -DDISABLE_HTTP=ON -DDISABLE_TTF=ON -DDISABLE_OPENGL=ON -DDISABLE_FLAC=ON -DDISABLE_VORBIS=ON \
  -DDISABLE_VERSION_CHECKER=ON -DDISABLE_DISCORD_RPC=ON -DDISABLE_GOOGLE_BENCHMARK=ON -DENABLE_SCRIPTING=OFF \
  -DWITH_TESTS=OFF -DDISABLE_IPO=ON -DDOWNLOAD_TITLE_SEQUENCES=OFF -DDOWNLOAD_OBJECTS=OFF -DDOWNLOAD_OPENSFX=OFF \
  -DDOWNLOAD_OPENMUSIC=OFF -DOPENRCT2_USE_CCACHE=OFF "$@"
