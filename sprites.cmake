# Generate the sprites metadata
set(g2_dat "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/g2.dat")
add_custom_command(
    OUTPUT "${g2_dat}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data"
    COMMAND $<TARGET_FILE:openrct2-cli> sprite build
        "${g2_dat}"
        ${PROJECT_SOURCE_DIR}/resources/g2/sprites.json
    DEPENDS
        openrct2-cli
        "${PROJECT_SOURCE_DIR}/resources/g2/sprites.json"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Generating g2.dat"
    )
add_custom_target(g2 ALL DEPENDS ${PROJECT} "${g2_dat}")
