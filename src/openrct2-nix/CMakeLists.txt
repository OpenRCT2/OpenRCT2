add_executable(openrct2-nix openrct2-nix.cpp)
target_link_libraries(openrct2-nix PRIVATE openrct2::ui)