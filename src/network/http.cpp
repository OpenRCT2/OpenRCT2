extern "C" {
	#include "http.h"
}

#ifdef DISABLE_HTTP

void http_init() { }
void http_dispose() { }

#else

#include <SDL.h>
#include <curl/curl.h>
#include <jansson/jansson.h>

typedef struct {
	char *ptr;
	int length;
	int capacity;
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

	int newBytesLength = size * nmemb;
	if (newBytesLength > 0) {
		int newCapacity = writeBuffer->capacity;
		int newLength = writeBuffer->length + newBytesLength;
		while (newLength > newCapacity) {
			newCapacity = max(4096, newCapacity * 2);
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

http_json_response *http_request_json(const char *url)
{
	CURL *curl;
	CURLcode curlResult;
	http_json_response *response;
	write_buffer writeBuffer;

	curl = curl_easy_init();
	if (curl == NULL)
		return NULL;

	writeBuffer.ptr = NULL;
	writeBuffer.length = 0;
	writeBuffer.capacity = 0;

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, TRUE);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
	curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeBuffer);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_request_write_func);

	curlResult = curl_easy_perform(curl);
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
		response->status_code = (int)httpStatusCode;
		response->root = root;
	}
	free(writeBuffer.ptr);
	return response;
}

void http_request_json_async(const char *url, void (*callback)(http_json_response*))
{
	struct TempThreadArgs {
		char *url;
		void (*callback)(http_json_response*);
	};

	TempThreadArgs *args = (TempThreadArgs*)malloc(sizeof(TempThreadArgs));
	args->url = _strdup(url);
	args->callback = callback;

	SDL_Thread *thread = SDL_CreateThread([](void *ptr) -> int {
		TempThreadArgs *args = (TempThreadArgs*)ptr;

		http_json_response *response = http_request_json(args->url);
		args->callback(response);

		free(args->url);
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