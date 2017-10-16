# Main executable for Unix and friends
add_executable(openrct2 openrct2-nix.cpp)
target_link_libraries(openrct2 PRIVATE openrct2::ui)
install(
    TARGETS openrct2
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
