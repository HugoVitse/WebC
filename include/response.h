#pragma once
#include "types.h"

typedef struct Response{

    Header* headers;
    int nbHeaders;

    int status;
    char* body;
    int bodyLen;

} Response;

Response* addServerHeaderToResponse(Server* server, Response* response);
Response* addHeaderToResponse(Header header, Response* response);
Response* addFileContentToResponseBody(const char* filename, Response* response);
Response* addContentToResponseBody(const char *content, Response *response);

char* stringifyResponse(Response* response, int* out_total_size);
void sendResponse(int new_socket, Response* reponse);
void freeResponse(Response* response);
