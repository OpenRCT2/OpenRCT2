# Generate the sprites metadata
if(MINGW AND NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Not building g2.dat: We're cross compiling with MinGW")
else()
    set(g2_dat "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/g2.dat")
    add_custom_command(
        OUTPUT "${g2_dat}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data"
        COMMAND $<TARGET_FILE:openrct2-cli> sprite build
            "${g2_dat}"
            ${PROJECT_SOURCE_DIR}/resources/g2/sprites.json
            silent
        DEPENDS
            openrct2-cli
            "${PROJECT_SOURCE_DIR}/resources/g2/sprites.json"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating g2.dat"
        )
    add_custom_target(g2 ALL DEPENDS "${g2_dat}")
    install(FILES "${g2_dat}" DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
endif()