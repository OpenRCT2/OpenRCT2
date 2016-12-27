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

#ifndef _HTTP_H_
#define _HTTP_H_

#ifndef DISABLE_HTTP
#include <jansson.h>
#include "../common.h"

typedef struct http_json_request {
	void *tag;
	const char *method;
	const char *url;
	const json_t *body;
} http_json_request;

typedef struct http_json_response {
	void *tag;
	int status_code;
	json_t *root;
} http_json_response;

#define HTTP_METHOD_GET		"GET"
#define HTTP_METHOD_POST	"POST"
#define HTTP_METHOD_PUT		"PUT"
#define HTTP_METHOD_DELETE	"DELETE"

http_json_response *http_request_json(const http_json_request *request);
void http_request_json_async(const http_json_request *request, void (*callback)(http_json_response*));
void http_request_json_dispose(http_json_response *response);
#endif // DISABLE_HTTP

// These callbacks are defined anyway, but are dummy if HTTP is disabled
void http_init();
void http_dispose();

#endif
