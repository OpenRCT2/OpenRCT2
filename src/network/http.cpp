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

#include "../core/Math.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"

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

static size_t http_request_read_func(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	read_buffer *readBuffer = (read_buffer*)userdata;

	size_t remainingBytes = readBuffer->length - readBuffer->position;
	size_t readBytes = size * nmemb;
	if (readBytes > remainingBytes) {
		readBytes = remainingBytes;
	}

	memcpy(ptr, readBuffer->ptr + readBuffer->position, readBytes);

	readBuffer->position += readBytes;
	return readBytes;
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

http_json_response *http_request_json(const http_json_request *request)
{
	CURL *curl;
	CURLcode curlResult;
	http_json_response *response;
	read_buffer readBuffer = { 0 };
	write_buffer writeBuffer;

	curl = curl_easy_init();
	if (curl == NULL)
		return NULL;

	if (request->body != NULL) {
		readBuffer.ptr = json_dumps(request->body, JSON_COMPACT);
		readBuffer.length = strlen(readBuffer.ptr);
		readBuffer.position = 0;
	}

	writeBuffer.ptr = NULL;
	writeBuffer.length = 0;
	writeBuffer.capacity = 0;

	curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: " MIME_TYPE_APPLICATION_JSON);
	if (request->body != NULL) {
		headers = curl_slist_append(headers, "Content-Type: " MIME_TYPE_APPLICATION_JSON);

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

	if (request->body != NULL) {
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

	curl_easy_cleanup(curl);

	// Null terminate the response buffer
	writeBuffer.length++;
	writeBuffer.ptr = (char*)realloc(writeBuffer.ptr, writeBuffer.length);
	writeBuffer.capacity = writeBuffer.length;
	writeBuffer.ptr[writeBuffer.length - 1] = 0;

	response = NULL;

	// Parse as JSON
	json_t *root;
	json_error_t error;
	root = json_loads(writeBuffer.ptr, 0, &error);
	if (root != NULL) {
		response = (http_json_response*)malloc(sizeof(http_json_response));
		response->tag = request->tag;
		response->status_code = (int)httpStatusCode;
		response->root = root;
	}
	free(writeBuffer.ptr);
	return response;
}

void http_request_json_async(const http_json_request *request, void (*callback)(http_json_response*))
{
	struct TempThreadArgs {
		http_json_request request;
		void (*callback)(http_json_response*);
	};

	TempThreadArgs *args = (TempThreadArgs*)malloc(sizeof(TempThreadArgs));
	args->request.url = _strdup(request->url);
	args->request.method = request->method;
	args->request.body = json_deep_copy(request->body);
	args->request.tag = request->tag;
	args->callback = callback;

	SDL_Thread *thread = SDL_CreateThread([](void *ptr) -> int {
		TempThreadArgs *args = (TempThreadArgs*)ptr;

		http_json_response *response = http_request_json(&args->request);
		args->callback(response);

		free((char*)args->request.url);
		json_decref((json_t*)args->request.body);
		free(args);
		return 0;
	}, NULL, args);

	if (thread == NULL) {
		log_error("Unable to create thread!");
		callback(NULL);
	} else {
		SDL_DetachThread(thread);
	}
}

void http_request_json_dispose(http_json_response *response)
{
	if (response->root != NULL)
		json_decref(response->root);

	free(response);
}

#endif
