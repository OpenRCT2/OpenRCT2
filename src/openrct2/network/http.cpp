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

#include <thread>

extern "C" {
    #include "http.h"
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

struct HttpRequest2
{
    void *          Tag = nullptr;
    std::string     Method;
    std::string     Url;
    http_data_type  Type;
    size_t          Size = 0;
    union
    {
        char *      Buffer = 0;
        json_t *    Json;
    } Body;

    HttpRequest2() { }

    HttpRequest2(const HttpRequest2 &request)
    {
        Tag = request.Tag;
        Method = request.Method;
        Url = request.Url;
        Type = request.Type;
        Size = request.Size;
        if (request.Type == HTTP_DATA_JSON)
        {
            Body.Json = json_deep_copy(request.Body.Json);
        }
        else
        {
            Body.Buffer = new char[request.Size];
            memcpy(Body.Buffer, request.Body.Buffer, request.Size);
        }
    }

    explicit HttpRequest2(const http_request_t * request)
    {
        Tag = request->tag;
        Method = std::string(request->method);
        Url = std::string(request->url);
        Type = request->type;
        Size = request->size;
        if (request->type == HTTP_DATA_JSON)
        {
            Body.Json = json_deep_copy(request->root);
        }
        else
        {
            Body.Buffer = new char[request->size];
            memcpy(Body.Buffer, request->body, request->size);
        }
    }

    ~HttpRequest2()
    {
        if (Type == HTTP_DATA_JSON)
        {
            json_decref(Body.Json);
        }
        else
        {
            delete Body.Buffer;
        }
    }
};

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

static http_response_t *http_request(const HttpRequest2 &request)
{
    CURL *curl;
    CURLcode curlResult;
    http_response_t *response;
    read_buffer readBuffer = { 0 };
    write_buffer writeBuffer;

    curl = curl_easy_init();
    if (curl == NULL)
        return NULL;

    if (request.Type == HTTP_DATA_JSON && request.Body.Json != NULL) {
        readBuffer.ptr = json_dumps(request.Body.Json, JSON_COMPACT);
        readBuffer.length = strlen(readBuffer.ptr);
        readBuffer.position = 0;
    } else if (request.Type == HTTP_DATA_RAW && request.Body.Buffer != NULL) {
        readBuffer.ptr = request.Body.Buffer;
        readBuffer.length = request.Size;
        readBuffer.position = 0;
    }

    writeBuffer.ptr = NULL;
    writeBuffer.length = 0;
    writeBuffer.capacity = 0;

    curl_slist *headers = NULL;

    if (request.Type == HTTP_DATA_JSON) {
        headers = curl_slist_append(headers, "Accept: " MIME_TYPE_APPLICATION_JSON);

        if (request.Body.Json != NULL) {
            headers = curl_slist_append(headers, "Content-Type: " MIME_TYPE_APPLICATION_JSON);
        }
    }

    if (readBuffer.ptr != NULL) {
        char contentLengthHeaderValue[64];
        snprintf(contentLengthHeaderValue, sizeof(contentLengthHeaderValue), "Content-Length: %zu", readBuffer.length);
        headers = curl_slist_append(headers, contentLengthHeaderValue);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readBuffer.ptr);
    }

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.Method.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, OPENRCT2_USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_URL, request.Url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeBuffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_request_write_func);

    curlResult = curl_easy_perform(curl);

    if (request.Type == HTTP_DATA_JSON && request.Body.Json != NULL) {
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
            response->tag = request.Tag;
            response->status_code = (sint32) httpStatusCode;
            response->root = root;
            response->type = HTTP_DATA_JSON;
            response->size = writeBuffer.length;
        }
        free(writeBuffer.ptr);
    } else {
        response = (http_response_t*) malloc(sizeof(http_response_t));
        response->tag = request.Tag;
        response->status_code = (sint32) httpStatusCode;
        response->body = writeBuffer.ptr;
        response->type = HTTP_DATA_RAW;
        response->size = writeBuffer.length;
    }

    curl_easy_cleanup(curl);

    return response;
}

void http_request_async(const http_request_t * request, void (*callback)(http_response_t*))
{
    auto request2 = HttpRequest2(request);
    auto thread = std::thread([](const HttpRequest2 &req, void(*callback2)(http_response_t*)) -> void
    {
        http_response_t * response = http_request(req);
        callback2(response);
    }, std::move(request2), callback);
    thread.detach();
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
	HttpRequest2 request;
	request.Url = url;
	request.Method = "GET";
	request.Type = HTTP_DATA_NONE;

	http_response_t *response = http_request(request);

	if (response == NULL || response->status_code != 200) {
		Console::Error::WriteLine("Failed to download '%s'", request.Url.c_str());
		if (response != NULL) {
			http_request_dispose(response);
		}
		return false;
	}

	// Generate temporary filename that includes the original extension
	if (tmpnam(tmpPath) == NULL) {
		Console::Error::WriteLine("Failed to generate temporary filename for downloaded park '%s'", request.Url.c_str());
		http_request_dispose(response);
		return false;
	}
	size_t remainingBytes = L_tmpnam + 10 - strlen(tmpPath);

	const char *ext = http_get_extension_from_url(request.Url.c_str(), ".sv6");
	strncat(tmpPath, ext, remainingBytes);

	// Store park in temporary file and load it (discard ending NUL in response body)
	FILE* tmpFile = fopen(tmpPath, "wb");

	if (tmpFile == NULL) {
		Console::Error::WriteLine("Failed to write downloaded park '%s' to temporary file", request.Url.c_str());
		http_request_dispose(response);
		return false;
	}

	fwrite(response->body, 1, response->size - 1, tmpFile);
	fclose(tmpFile);

	http_request_dispose(response);

	return true;
}

#endif
