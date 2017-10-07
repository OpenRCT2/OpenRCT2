# Sources
file(GLOB_RECURSE OPENRCT2_CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.h"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
if (APPLE)
    file(GLOB_RECURSE OPENRCT2_CORE_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.m")
    set_source_files_properties(${OPENRCT2_CORE_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c -fmodules")
endif ()

# Handle creating the rct2 text and data files on macOS and Linux
# See details in src/openrct2/rct2/interop.c:rct2_interop_setup_segment for how the values
# were derived.
if (ENABLE_RCT2 AND UNIX)
    set(OPENRCT2_EXE "${PROJECT_SOURCE_DIR}/openrct2.exe")
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
    if (NOT USE_MMAP)
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
            set(RCT2_SEGMENT_LINKER_FLAGS "-Wl,-T,\"${PROJECT_SOURCE_DIR}/distribution/linux/${LINKER_SCRIPT}\"")
        endif ()
    endif ()
elseif (USE_MMAP)
    # No dd here, can't extract data segment
    message(WARNING "Sorry, your platform is not supported, you have to extract data segment manually")
endif ()

# Outputs
add_library(libopenrct2 ${OPENRCT2_CORE_SOURCES} ${OPENRCT2_CORE_MM_SOURCES} ${RCT2_SECTIONS})
add_library(openrct2::core ALIAS libopenrct2)
target_link_libraries(libopenrct2 PUBLIC openrct2::flags)
target_include_directories(libopenrct2 PUBLIC ${OPENRCT2_SOURCE_DIR})
# Don't add a redundant library prefix
set_target_properties(libopenrct2 PROPERTIES PREFIX "")

if (ENABLE_RCT2)
    add_dependencies(libopenrct2 segfiles)
    if(NOT USE_MMAP)
        if(NOT APPLE)
            add_dependencies(libopenrct2 linkable_sections)
        endif()
        set_target_properties(libopenrct2 PROPERTIES LINK_FLAGS ${RCT2_SEGMENT_LINKER_FLAGS})
    endif()
endif()

if (APPLE AND NOT USE_MMAP)
    set(PIE_FLAG "-fno-pie")
else ()
    set(PIE_FLAG "-fpie")
endif ()


if (ENABLE_RCT2)
    # Disable optimizations for addresses.c for all compilers, to allow optimized
    # builds without need for -fno-omit-frame-pointer
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/rct2/addresses.c PROPERTIES COMPILE_FLAGS -O0)
endif ()

if (CXX_WARN_SUGGEST_FINAL_TYPES)
    # Disable -Wsuggest-final-types via pragmas where due.
   add_definitions(-D__WARN_SUGGEST_FINAL_TYPES__)
endif ()

if (CXX_WARN_SUGGEST_FINAL_METHODS)
    # Disable -Wsuggest-final-methods via pragmas where due.
   add_definitions(-D__WARN_SUGGEST_FINAL_METHODS__)
endif ()

set_property(SOURCE Version.cpp APPEND PROPERTY COMPILE_DEFINITIONS
    OPENRCT2_BRANCH=\"${OPENRCT2_BRANCH}\"
    OPENRCT2_COMMIT_SHA1_SHORT=\"${OPENRCT2_SHA1_SHORT}\"
    )