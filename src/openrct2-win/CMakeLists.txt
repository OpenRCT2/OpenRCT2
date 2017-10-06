add_executable(openrct2-win openrct2-win.cpp ${PROJECT_SOURCE_DIR}/resources/OpenRCT2.rc)
target_link_libraries(openrct2-win PRIVATE openrct2-dll)