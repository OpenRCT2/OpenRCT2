# CMake toolchain file for m68k-amigaos (AmigaOS 3.x) with bebbo's amiga-gcc.
#
#   cmake -S . -B build-68k -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/m68k-amigaos.cmake \
#         -DAMIGA_PREFIX=$HOME/opt/amiga16 -DAMIGA_DEPS=/path/to/cross-built/deps ...
#
# See distribution/amiga/BUILDING.md for the complete recipe (compiler build,
# dependencies, configure flags).
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR m68k)
set(AMIGA_PREFIX "$ENV{HOME}/opt/amiga16" CACHE PATH "amiga-gcc install prefix")
set(CMAKE_C_COMPILER   ${AMIGA_PREFIX}/bin/m68k-amigaos-gcc)
set(CMAKE_CXX_COMPILER ${AMIGA_PREFIX}/bin/m68k-amigaos-g++)
set(CMAKE_AR           ${AMIGA_PREFIX}/bin/m68k-amigaos-ar)
set(CMAKE_RANLIB       ${AMIGA_PREFIX}/bin/m68k-amigaos-ranlib)
# 68020 code with soft float runs on every accelerated Amiga, on PiStorm/Emu68
# (which presents a 68040 with an incomplete FPU) and on the Vampire.
set(AMIGA_CPU_FLAGS "-m68020 -msoft-float" CACHE STRING "CPU/FPU flags")
# bebbo's GCC defines int32_t as `long`; OpenRCT2 relies on int32_t == int
# (std::min/max/clamp deduction, overloads), so the builtin type macros are
# redefined before every translation unit.
set(CMAKE_C_FLAGS_INIT   "${AMIGA_CPU_FLAGS} -fsigned-char -mcrt=nix20")
set(CMAKE_CXX_FLAGS_INIT "${AMIGA_CPU_FLAGS} -fsigned-char -mcrt=nix20 -include ${CMAKE_CURRENT_LIST_DIR}/m68k-amigaos-int32.h")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-mcrt=nix20")
# libpthread/libatomic must come after every object and library.
set(CMAKE_CXX_STANDARD_LIBRARIES "-lpthread -latomic" CACHE STRING "")
set(CMAKE_C_STANDARD_LIBRARIES "-lpthread -latomic" CACHE STRING "")
set(AMIGA_DEPS "$ENV{AMIGA_DEPS}" CACHE PATH "prefix of the cross-built dependencies (zlib, zstd, libpng, libzip)")
set(CMAKE_FIND_ROOT_PATH ${AMIGA_PREFIX}/m68k-amigaos ${AMIGA_DEPS})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(AMIGA ON CACHE BOOL "Building for AmigaOS")
