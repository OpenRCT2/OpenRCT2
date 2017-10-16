add_executable(openrct2-cli Cli.cpp)
target_link_libraries(openrct2-cli PRIVATE openrct2::core)
if(MSVC)
    target_sources(openrct2-cli PRIVATE ${PROJECT_SOURCE_DIR}/resources/OpenRCT2.rc)
endif()
install(
    TARGETS openrct2-cli
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
