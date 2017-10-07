# Defines tweak-able build options

include(CMakeDependentOption)

if(WIN32)
    set(is_windows TRUE)
else()
    set(is_windows FALSE)
endif()

# Various build options
set(DEBUG_LEVEL 0 CACHE INT "Select debug level for compilation. Use value in range 0–3.")
set_property(CACHE DEBUG_LEVEL PROPERTY STRINGS 0 1 2 3)
option(STATIC_LINK_EXECUTABLE "Link dependent libraries into the executable statically" ON)

if(NOT MSVC)
    option(FORCE_32BIT "Force compile for 32-bit architectures" NO)
endif()

# Options for using original rct2 binaries
option(ENABLE_RCT2 "Use code and data segments from the original executable binayr" OFF)
cmake_dependent_option(USE_MMAP_FOR_RCT2 "Use mmap to load rct2's data segment into memory" ON "ENABLE_RCT2" OFF)

# Features
option(ENABLE_HTTP_TWITCH "Disable HTTP and Twitch support." ON)
option(ENABLE_TTF "Enable support for TTF provided by freetype2" ON)
option(ENABLE_LIGHTFX "Enable lighting effects" ON)
option(ENABLE_NETWORK "Enable multiplayer networking functionality." ON)
option(ENABLE_OPENGL "Enable OpenGL support" OFF)