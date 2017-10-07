# Main runtime code for Windows. Separate from the actual main() entry point.
add_library(openrct2-dll SHARED openrct2-dll.cpp)
add_library(openrct2::dll ALIAS openrct2-dll)
target_link_libraries(openrct2-dll PRIVATE openrct2::ui)
# Remove the `-dll` from the filename
set_property(TARGET openrct2-dll PROPERTY OUTPUT_NAME openrct2)
install(
    TARGETS openrct2-dll
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
