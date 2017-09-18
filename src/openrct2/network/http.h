#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _HTTP_H_
#define _HTTP_H_

#ifdef __cplusplus

#ifndef DISABLE_HTTP
#include <jansson.h>
#include "../common.h"

typedef enum http_data_type_T {
    HTTP_DATA_NONE,
    HTTP_DATA_RAW,
    HTTP_DATA_JSON
} http_data_type;

typedef struct http_request_t {
    void *tag;
    const char *method;
    const char *url;
    http_data_type type = HTTP_DATA_NONE;
    size_t size;
    union {
        const json_t *root;
        char* body;
    };
} http_request_t;

typedef struct http_response_t {
    void *tag;
    sint32 status_code;
    http_data_type type;
    size_t size;
    union {
        json_t *root;
        char* body;
    };
} http_response_t;

#define HTTP_METHOD_GET     "GET"
#define HTTP_METHOD_POST    "POST"
#define HTTP_METHOD_PUT     "PUT"
#define HTTP_METHOD_DELETE  "DELETE"

void http_request_async(const http_request_t *request, void (*callback)(http_response_t*));
void http_request_dispose(http_response_t *response);

const char *http_get_extension_from_url(const char *url, const char *fallback);

/**
 * Download a park via HTTP/S from the given URL into a memory buffer. This is
 * a blocking operation.
 * @param url The URL to download the park from.
 * @param outData The data returned.
 * @returns The size of the data or 0 if the download failed.
 */
size_t http_download_park(const char * url, void * * outData);
#endif // DISABLE_HTTP

// These callbacks are defined anyway, but are dummy if HTTP is disabled
void http_init();
void http_dispose();

#endif

#endif
