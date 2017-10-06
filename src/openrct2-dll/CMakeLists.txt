add_library(openrct2-dll openrct2-dll.cpp)
target_link_libraries(openrct2-dll PRIVATE openrct2::ui)
