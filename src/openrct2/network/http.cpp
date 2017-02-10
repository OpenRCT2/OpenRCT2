#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

extern "C" {
    #include "http.h"
    #include "../platform/platform.h"
}

#ifdef DISABLE_HTTP

void http_init() { }
void http_dispose() { }

#else

#include "../core/Console.hpp"
#include "../core/Math.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../Version.h"

#ifdef __WINDOWS__
    // cURL includes windows.h, but we don't need all of it.
    #define WIN32_LEAN_AND_MEAN
#endif
#include <curl/curl.h>

#define MIME_TYPE_APPLICATION_JSON "application/json"
#define OPENRCT2_USER_AGENT "OpenRCT2/" OPENRCT2_VERSION

typedef struct read_buffer {
    char *ptr;
    size_t length;
    size_t position;
} read_buffer;

typedef struct write_buffer {
    char *ptr;
    size_t length;
    size_t capacity;
} write_buffer;

void http_init()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void http_dispose()
{
    curl_global_cleanup();
}

static size_t http_request_write_func(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    write_buffer *writeBuffer = (write_buffer*)userdata;

    size_t newBytesLength = size * nmemb;
    if (newBytesLength > 0) {
        size_t newCapacity = writeBuffer->capacity;
        size_t newLength = writeBuffer->length + newBytesLength;
        while (newLength > newCapacity) {
            newCapacity = Math::Max<size_t>(4096, newCapacity * 2);
        }
        if (newCapacity != writeBuffer->capacity) {
            writeBuffer->ptr = (char*)realloc(writeBuffer->ptr, newCapacity);
            writeBuffer->capacity = newCapacity;
        }

        memcpy(writeBuffer->ptr + writeBuffer->length, ptr, newBytesLength);
        writeBuffer->length = newLength;
    }
    return newBytesLength;
}

http_response_t *http_request(const http_request_t *request)
{
    CURL *curl;
    CURLcode curlResult;
    http_response_t *response;
    read_buffer readBuffer = { 0 };
    write_buffer writeBuffer;

    curl = curl_easy_init();
    if (curl == NULL)
        return NULL;

    if (request->type == HTTP_DATA_JSON && request->root != NULL) {
        readBuffer.ptr = json_dumps(request->root, JSON_COMPACT);
        readBuffer.length = strlen(readBuffer.ptr);
        readBuffer.position = 0;
    } else if (request->type == HTTP_DATA_RAW && request->body != NULL) {
        readBuffer.ptr = request->body;
        readBuffer.length = request->size;
        readBuffer.position = 0;
    }

    writeBuffer.ptr = NULL;
    writeBuffer.length = 0;
    writeBuffer.capacity = 0;

    curl_slist *headers = NULL;

    if (request->type == HTTP_DATA_JSON) {
        headers = curl_slist_append(headers, "Accept: " MIME_TYPE_APPLICATION_JSON);

        if (request->root != NULL) {
            headers = curl_slist_append(headers, "Content-Type: " MIME_TYPE_APPLICATION_JSON);
        }
    }

    if (readBuffer.ptr != NULL) {
        char contentLengthHeaderValue[64];
        snprintf(contentLengthHeaderValue, sizeof(contentLengthHeaderValue), "Content-Length: %zu", readBuffer.length);
        headers = curl_slist_append(headers, contentLengthHeaderValue);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readBuffer.ptr);
    }

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request->method);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, OPENRCT2_USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_URL, request->url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeBuffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_request_write_func);

    curlResult = curl_easy_perform(curl);

    if (request->type == HTTP_DATA_JSON && request->root != NULL) {
        free(readBuffer.ptr);
    }

    if (curlResult != CURLE_OK) {
        log_error("HTTP request failed: %s.", curl_easy_strerror(curlResult));
        if (writeBuffer.ptr != NULL)
            free(writeBuffer.ptr);

        return NULL;
    }

    long httpStatusCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

    char* contentType;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);

    // Null terminate the response buffer
    writeBuffer.length++;
    writeBuffer.ptr = (char*)realloc(writeBuffer.ptr, writeBuffer.length);
    writeBuffer.capacity = writeBuffer.length;
    writeBuffer.ptr[writeBuffer.length - 1] = 0;

    response = NULL;

    // Parse as JSON if response is JSON
    if (contentType != NULL && strstr(contentType, "json") != NULL) {
        json_t *root;
        json_error_t error;
        root = json_loads(writeBuffer.ptr, 0, &error);
        if (root != NULL) {
            response = (http_response_t*) malloc(sizeof(http_response_t));
            response->tag = request->tag;
            response->status_code = (sint32) httpStatusCode;
            response->root = root;
            response->type = HTTP_DATA_JSON;
            response->size = writeBuffer.length;
        }
        free(writeBuffer.ptr);
    } else {
        response = (http_response_t*) malloc(sizeof(http_response_t));
        response->tag = request->tag;
        response->status_code = (sint32) httpStatusCode;
        response->body = writeBuffer.ptr;
        response->type = HTTP_DATA_RAW;
        response->size = writeBuffer.length;
    }

    curl_easy_cleanup(curl);

    return response;
}

void http_request_async(const http_request_t *request, void (*callback)(http_response_t*))
{
    struct TempThreadArgs {
        http_request_t request;
        void (*callback)(http_response_t*);
    };

    TempThreadArgs *args = (TempThreadArgs*)malloc(sizeof(TempThreadArgs));
    args->request.url = _strdup(request->url);
    args->request.method = request->method;

    if (request->type == HTTP_DATA_JSON) {
        args->request.root = json_deep_copy(request->root);
    } else {
        char* bodyCopy = (char*) malloc(request->size);
        memcpy(bodyCopy, request->body, request->size);
        args->request.body = bodyCopy;
    }

    args->request.type = request->type;
    args->request.size = request->size;
    args->request.tag = request->tag;
    args->callback = callback;

    SDL_Thread *thread = SDL_CreateThread([](void *ptr) -> sint32 {
        TempThreadArgs *args2 = (TempThreadArgs*)ptr;

        http_response_t *response = http_request(&args2->request);
        args2->callback(response);

        free((char*)args2->request.url);

        if (args2->request.type == HTTP_DATA_JSON) {
            json_decref((json_t*) args2->request.root);
        } else {
            free(args2->request.body);
        }

        free(args2);
        return 0;
    }, NULL, args);

    if (thread == NULL) {
        log_error("Unable to create thread!");
        callback(NULL);
    } else {
        SDL_DetachThread(thread);
    }
}

void http_request_dispose(http_response_t *response)
{
    if (response->type == HTTP_DATA_JSON && response->root != NULL)
        json_decref(response->root);
    else if (response->type == HTTP_DATA_RAW && response->body != NULL)
        free(response->body);

    free(response);
}

const char *http_get_extension_from_url(const char *url, const char *fallback)
{
    const char *extension = strrchr(url, '.');

    // Assume a save file by default if no valid extension can be determined
    if (extension == NULL || strchr(extension, '/') != NULL) {
        return fallback;
    } else {
        return extension;
    }
}

bool http_download_park(const char *url, char tmpPath[L_tmpnam + 10])
{
	// Download park to buffer in memory
	http_request_t request;
	request.url = url;
	request.method = "GET";
	request.type = HTTP_DATA_NONE;

	http_response_t *response = http_request(&request);

	if (response == NULL || response->status_code != 200) {
		Console::Error::WriteLine("Failed to download '%s'", request.url);
		if (response != NULL) {
			http_request_dispose(response);
		}
		return false;
	}

	// Generate temporary filename that includes the original extension
	if (tmpnam(tmpPath) == NULL) {
		Console::Error::WriteLine("Failed to generate temporary filename for downloaded park '%s'", request.url);
		http_request_dispose(response);
		return false;
	}
	size_t remainingBytes = L_tmpnam + 10 - strlen(tmpPath);

	const char *ext = http_get_extension_from_url(request.url, ".sv6");
	strncat(tmpPath, ext, remainingBytes);

	// Store park in temporary file and load it (discard ending NUL in response body)
	FILE* tmpFile = fopen(tmpPath, "wb");

	if (tmpFile == NULL) {
		Console::Error::WriteLine("Failed to write downloaded park '%s' to temporary file", request.url);
		http_request_dispose(response);
		return false;
	}

	fwrite(response->body, 1, response->size - 1, tmpFile);
	fclose(tmpFile);

	http_request_dispose(response);

	return true;
}

#endif
