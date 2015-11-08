#ifndef _HTTP_H_
#define _HTTP_H_

#ifndef DISABLE_HTTP
#include <jansson.h>
#include "../common.h"

typedef struct {
	const char *method;
	const char *url;
	const json_t *body;
} http_json_request;

typedef struct {
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
