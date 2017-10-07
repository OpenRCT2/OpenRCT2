# Main executable for Unix and friends
add_executable(openrct2-nix openrct2-nix.cpp)
target_link_libraries(openrct2-nix PRIVATE openrct2::ui)
# Remove the `-nix` from the filename
set_property(TARGET openrct2-nix PROPERTY OUTPUT_NAME openrct2)
install(
    TARGETS openrct2-nix
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
