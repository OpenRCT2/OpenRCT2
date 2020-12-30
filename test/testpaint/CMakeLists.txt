# CMAKE project for testpaint
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

set(OPENRCT2_EXE "${ROOT_DIR}/openrct2.exe")
add_custom_command(
    OUTPUT openrct2_text
    COMMAND dd if="${OPENRCT2_EXE}" of="${CMAKE_BINARY_DIR}/openrct2_text" bs=4096 skip=1 count=1187
    DEPENDS ${OPENRCT2_EXE}
)
add_custom_command(
    OUTPUT openrct2_data
    COMMAND dd if="${OPENRCT2_EXE}" of="${CMAKE_BINARY_DIR}/openrct2_data" bs=4096 skip=1188 count=318
    COMMAND dd if=/dev/zero of="${CMAKE_BINARY_DIR}/openrct2_data" bs=4096 seek=318 count=2630 conv=notrunc
    COMMAND dd if="${OPENRCT2_EXE}" of="${CMAKE_BINARY_DIR}/openrct2_data" bs=4096 skip=1506 seek=2948 count=1 conv=notrunc
    DEPENDS ${OPENRCT2_EXE}
)
add_custom_target(segfiles DEPENDS openrct2_text openrct2_data)
if (NOT USE_MMAP AND FORCE32)
    set(OBJ_FORMAT "elf32-i386")
    set(LINKER_SCRIPT "ld_script_i386.xc")
    if (APPLE)
        set(RCT2_SEGMENT_LINKER_FLAGS "-sectcreate rct2_text __text ${CMAKE_BINARY_DIR}/openrct2_text -sectcreate rct2_data __data ${CMAKE_BINARY_DIR}/openrct2_data -segaddr rct2_data 0x8a4000 -segprot rct2_data rwx rwx -segaddr rct2_text 0x401000 -segprot rct2_text rwx rwx -segaddr __TEXT 0x2000000 -read_only_relocs suppress")
    else ()
        # For Linux we have to use objcopy to wrap regular binaries into a linkable
        # format. We use specific section names which are then referenced in a
        # bespoke linker script so they can be placed at predefined VMAs.
        add_custom_command(
            OUTPUT openrct2_text_section.o
            COMMAND objcopy --input binary --output ${OBJ_FORMAT} --binary-architecture i386 openrct2_text openrct2_text_section.o --rename-section .data=.rct2_text,contents,alloc,load,readonly,code
            DEPENDS segfiles
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        add_custom_command(
            OUTPUT openrct2_data_section.o
            COMMAND objcopy --input binary --output ${OBJ_FORMAT} --binary-architecture i386 openrct2_data openrct2_data_section.o --rename-section .data=.rct2_data,contents,alloc,load,readonly,data
            DEPENDS segfiles
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        add_custom_target(linkable_sections DEPENDS openrct2_text_section.o openrct2_data_section.o)
        set_source_files_properties(
            openrct2_text_section.o openrct2_data_section.o
            PROPERTIES
            EXTERNAL_OBJECT true
            GENERATED true
        )
        # can't use GLOB here, as the files don't exist yet at cmake-time
        set(RCT2_SECTIONS "${CMAKE_BINARY_DIR}/openrct2_data_section.o" "${CMAKE_BINARY_DIR}/openrct2_text_section.o")
        set(RCT2_SEGMENT_LINKER_FLAGS "-Wl,-T,\"${ROOT_DIR}/distribution/linux/${LINKER_SCRIPT}\"")
    endif ()
endif ()

set(OPENRCT2_SRCPATH "${ROOT_DIR}/src/openrct2")
file(GLOB_RECURSE ORCT2_RIDE_SOURCES "${OPENRCT2_SRCPATH}/ride/*/*.cpp")
file(GLOB_RECURSE ORCT2_RIDE_DEP_SOURCES
    "${OPENRCT2_SRCPATH}/Diagnostic.cpp"
    "${OPENRCT2_SRCPATH}/paint/PaintHelpers.cpp"
    "${OPENRCT2_SRCPATH}/paint/tile_element/Paint.TileElement.cpp"
    "${OPENRCT2_SRCPATH}/ride/RideData.cpp"
    "${OPENRCT2_SRCPATH}/ride/TrackData.cpp"
    "${OPENRCT2_SRCPATH}/ride/TrackPaint.cpp"
    "${OPENRCT2_SRCPATH}/core/Console.cpp"
    "${OPENRCT2_SRCPATH}/core/Diagnostics.cpp"
    "${OPENRCT2_SRCPATH}/core/Guard.cpp"
    "${OPENRCT2_SRCPATH}/core/String.cpp"
    "${OPENRCT2_SRCPATH}/Diagnostic.cpp"
    "${OPENRCT2_SRCPATH}/interface/ZoomLevel.cpp"
    "${OPENRCT2_SRCPATH}/localisation/ConversionTables.cpp"
    "${OPENRCT2_SRCPATH}/localisation/Convert.cpp"
    "${OPENRCT2_SRCPATH}/localisation/FormatCodes.cpp"
    "${OPENRCT2_SRCPATH}/localisation/UTF8.cpp"
    "${OPENRCT2_SRCPATH}/util/Util.cpp"
    "${OPENRCT2_SRCPATH}/Version.cpp"
)
file(GLOB_RECURSE ORCT2_TESTPAINT_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c"
                                          "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                          "${CMAKE_CURRENT_LIST_DIR}/*.h")

# Disable optimizations for addresses.c for all compilers, to allow optimized
# builds without need for -fno-omit-frame-pointer
set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/addresses.c PROPERTIES COMPILE_FLAGS -O0)

add_executable(testpaint EXCLUDE_FROM_ALL ${ORCT2_RIDE_SOURCES} ${ORCT2_RIDE_DEP_SOURCES} ${ORCT2_TESTPAINT_SOURCES} ${RCT2_SECTIONS})
SET_CHECK_CXX_FLAGS(testpaint)
target_compile_options(testpaint PRIVATE -Wno-old-style-cast)
target_include_directories(testpaint PRIVATE "${ROOT_DIR}/src/")
target_link_libraries(testpaint z)

if (NOT MINGW AND NOT MSVC)
    # For unicode code page conversion
    find_package(ICU 59.0 REQUIRED COMPONENTS uc)
    target_link_libraries(testpaint ${ICU_LIBRARIES})
    target_include_directories(testpaint SYSTEM PUBLIC ${ICU_INCLUDE_DIRS})
endif ()

# Only use custom linker script for 32 bit builds. For 64 bit builds, it should still _compile_.
if (FORCE32)
    set_target_properties(testpaint PROPERTIES LINK_FLAGS ${RCT2_SEGMENT_LINKER_FLAGS})
else ()
    set(TESTPAINT_64BIT_FLAGS "-Wno-int-to-pointer-cast -fpermissive -Wno-error")
endif ()
set_target_properties(testpaint PROPERTIES COMPILE_FLAGS "-DNO_VEHICLES -D__TESTPAINT__ -Wno-unused ${TESTPAINT_64BIT_FLAGS} -DDISABLE_HTTP")
add_dependencies(testpaint segfiles)
