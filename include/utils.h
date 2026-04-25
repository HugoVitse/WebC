#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "server.h"

Response* addFileContentToResponseBody(const char* filename, Response* response);
Response* addContentToResponseBody(const char *content, Response *response);

void sendResponse(int new_socket, Response* reponse);


typedef struct Parameter {
    char* name;
    char* value;
} Parameter;

typedef struct ParsedBody{
    Parameter* params;
    int nbParams;
} ParsedBody;

ParsedBody* parseBody(char* body);

char* getValue(ParsedBody* body, char* value);
