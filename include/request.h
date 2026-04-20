#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "method.h"
#include "server.h"

typedef struct Request {

    enum METHOD method;
    Header* headers;
    int nbHeaders;
    char* route;

}Request;


void freeReq(Request* req);
Request* parseRequest(char* buffer);
