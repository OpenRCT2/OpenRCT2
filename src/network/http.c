#include <curl/curl.h>
#include "http.h"

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
			writeBuffer->ptr = realloc(writeBuffer->ptr, newCapacity);
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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	// TODO set certificate bundle
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
	writeBuffer.ptr = realloc(writeBuffer.ptr, writeBuffer.length);
	writeBuffer.capacity = writeBuffer.length;
	writeBuffer.ptr[writeBuffer.length - 1] = 0;

	response = malloc(sizeof(http_json_response));
	response->status_code = (int)httpStatusCode;
	response->rawResponse = writeBuffer.ptr;
	return response;
}

void http_request_json_dispose(http_json_response *response)
{
	if (response->rawResponse != NULL)
		free(response->rawResponse);

	free(response);
}