add_executable(openrct2-cli Cli.cpp)

if(MSVC)
    target_sources(openrct2-cli PRIVATE ${PROJECT_SOURCE_DIR}/resources/OpenRCT2.rc)
endif()

target_link_libraries(openrct2-cli PUBLIC libopenrct2)

# Includes
target_include_directories(openrct2-cli PRIVATE "${OPENRCT2_SOURCE_DIR}")
