set(TITLE_SEQUENCE_URL  "https://github.com/OpenRCT2/title-sequences/releases/download/v0.1.0/title-sequence-v0.1.0.zip")
set(dest "${SCRATCH_DIR}")
file(DOWNLOAD "${TITLE_SEQUENCE_URL}" "${SCRATCH_DIR}/title-sequences.zip")
file(MAKE_DIRECTORY "${DEST}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E chdir "${DEST}"
        "${CMAKE_COMMAND}" -E tar xvf "${SCRATCH_DIR}/title-sequences.zip"
    RESULT_VARIABLE res
    )

if(res)
    message(FATAL_ERROR "Failed to extract title sequence [${res}]")
endif()