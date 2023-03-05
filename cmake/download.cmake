function(download_openrct2_zip)
    set(oneValueArgs ZIP_VERSION DOWNLOAD_DIR ZIP_URL SHA1)
    set(multiValueArgs SKIP_IF_EXISTS)
    cmake_parse_arguments(DOWNLOAD_OPENRCT2 "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN} )

    get_filename_component(ZIP_FILE_NAME ${DOWNLOAD_OPENRCT2_ZIP_URL} NAME)

    if (NOT EXISTS ${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR})
        set(DOWNLOAD_ZIP 1)
    else ()
        if (EXISTS "${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}/${ZIP_FILE_NAME}.zipversion")
            file(READ "${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}/${ZIP_FILE_NAME}.zipversion" DOWNLOAD_OPENRCT2_CACHED_VERSION)
            if (NOT ${DOWNLOAD_OPENRCT2_CACHED_VERSION} STREQUAL ${DOWNLOAD_OPENRCT2_ZIP_VERSION})
                message("Cache ${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR} not up to date")
                set(DOWNLOAD_ZIP 1)
            endif ()
        else ()
            set(DOWNLOAD_ZIP 1)
        endif ()
    endif ()

    foreach(check_exist_file_or_dir ${DOWNLOAD_OPENRCT2_SKIP_IF_EXISTS})
        if (EXISTS ${check_exist_file_or_dir})
            message("${check_exist_file_or_dir} exists, skipping download")
            set(DOWNLOAD_ZIP 0)
            break()
        endif ()
    endforeach(check_exist_file_or_dir)

    if (DOWNLOAD_ZIP)
        message("Downloading ${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}")
        file(DOWNLOAD
            "${DOWNLOAD_OPENRCT2_ZIP_URL}" "${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}/${ZIP_FILE_NAME}"
            EXPECTED_HASH SHA1=${DOWNLOAD_OPENRCT2_SHA1} SHOW_PROGRESS)
        if(${CMAKE_VERSION} VERSION_LESS "3.18.0") 
            execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR} ${CMAKE_COMMAND} -E tar xf ${ZIP_FILE_NAME})
        else()
            file(ARCHIVE_EXTRACT
                INPUT "${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}/${ZIP_FILE_NAME}"
                DESTINATION "${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}"
            )
        endif()
        file(WRITE
            "${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}/${ZIP_FILE_NAME}.zipversion"
            "${DOWNLOAD_OPENRCT2_ZIP_VERSION}"
        )
        file(REMOVE "${DOWNLOAD_OPENRCT2_DOWNLOAD_DIR}/${ZIP_FILE_NAME}")
    endif ()

endfunction ()
