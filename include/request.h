#pragma once
#include "types.h"


typedef struct Request {

    enum METHOD method;
    Header* headers;
    int nbHeaders;
    char* route;
    char* body;

}Request;


void freeReq(Request* req);
Request* parseRequest(char* buffer);
