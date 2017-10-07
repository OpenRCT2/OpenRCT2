add_library(openrct2-dll SHARED openrct2-dll.cpp)
target_link_libraries(openrct2-dll PRIVATE openrct2::ui)
set_property(TARGET openrct2-dll PROPERTY OUTPUT_NAME openrct2)
install(
    TARGETS openrct2-dll
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
