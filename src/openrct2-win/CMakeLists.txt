# Windows main program
add_executable(openrct2 openrct2-win.cpp ${PROJECT_SOURCE_DIR}/resources/OpenRCT2.rc)
target_link_libraries(openrct2 PRIVATE openrct2::dll)
install(
    TARGETS openrct2
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
