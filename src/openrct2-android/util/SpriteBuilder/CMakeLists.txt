cmake_minimum_required(VERSION 3.6.0)
project(SpriteBuilder CXX)

set(CMAKE_CXX_STANDARD 17)

set(ROOT_DIR "${CMAKE_SOURCE_DIR}/../../../../")
include_directories("${ROOT_DIR}/src")
include_directories("${ROOT_DIR}/src/thirdparty")
# Directory installed in OpenRCT2's Android Docker images
include_directories("/nlohmann/../")

# Some most common files required in tests
set(SB_SOURCES
    "${ROOT_DIR}/src/openrct2/cmdline/SpriteCommands.cpp"
    "${ROOT_DIR}/src/openrct2/CmdlineSprite.cpp"
    "${ROOT_DIR}/src/openrct2/core/Console.cpp"
    "${ROOT_DIR}/src/openrct2/core/Diagnostics.cpp"
    "${ROOT_DIR}/src/openrct2/core/File.cpp"
    "${ROOT_DIR}/src/openrct2/core/FileStream.cpp"
    "${ROOT_DIR}/src/openrct2/core/Guard.cpp"
    "${ROOT_DIR}/src/openrct2/core/Imaging.cpp"
    "${ROOT_DIR}/src/openrct2/core/Json.cpp"
    "${ROOT_DIR}/src/openrct2/core/Path.cpp"
    "${ROOT_DIR}/src/openrct2/core/String.cpp"
    "${ROOT_DIR}/src/openrct2/Diagnostic.cpp"
    "${ROOT_DIR}/src/openrct2/drawing/Drawing.cpp"
    "${ROOT_DIR}/src/openrct2/drawing/ImageImporter.cpp"
    "${ROOT_DIR}/src/openrct2/drawing/NewDrawing.cpp"
    "${ROOT_DIR}/src/openrct2/interface/ZoomLevel.cpp"
    "${ROOT_DIR}/src/openrct2/localisation/ConversionTables.cpp"
    "${ROOT_DIR}/src/openrct2/localisation/FormatCodes.cpp"
    "${ROOT_DIR}/src/openrct2/localisation/UTF8.cpp"
    "${ROOT_DIR}/src/openrct2/platform/Platform.Posix.cpp"
    "${ROOT_DIR}/src/openrct2/platform/Posix.cpp"
    "${ROOT_DIR}/src/openrct2/platform/Shared.cpp"
    "${ROOT_DIR}/src/openrct2/util/Util.cpp"
    "${ROOT_DIR}/src/openrct2/Version.cpp"
    "sb.cpp"
    )

add_executable(sb ${SB_SOURCES})
set_target_properties(sb PROPERTIES COMPILE_DEFINITIONS "DISABLE_HTTP;SPRITE_BUILDER")
find_package(PNG REQUIRED)
target_link_libraries(sb ${PNG_LIBRARIES})
