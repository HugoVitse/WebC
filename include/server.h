#pragma once
#include "method.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



typedef struct Header {
    char* header;
    char* value;
}Header;

typedef struct Request {

    enum METHOD method;
    Header* headers;
    int nbHeaders;
    char* route;
    char* body;

}Request;


typedef struct Response{

    Header* headers;
    int nbHeaders;

    int status;
    char* body;

} Response;

typedef struct Route {
    char* stringRoute;
    enum METHOD verb;
    Response* (*method)(Response*, Request*);
}Route;



typedef struct Server {

    int port;
    struct sockaddr_in address;

    Header* headers;
    Route* routes;

    int nbHeaders;
    int nbRoutes;

    int server_fd;

    char* staticRoute;
    char* staticPath;

    Response* (*defautlMethod)(Response*, Request*);

}Server;



void freeReq(Request* req);
Request* parseRequest(char* buffer);

void addGlobalHeader(Server* server, const char* header, const char* value);
void addRoute(Server* server, const char* route, Response* (*method)(Response*, Request*), enum METHOD verb);
Response* addServerHeaderToResponse(Server* server, Response* response);
Response* addHeaderToResponse(Header header, Response* response);
char* stringifyResponse(Response* response);

void startServer(Server* server);
void freeServer(Server* serv);
void run(Server* serv);
void stop(Server* serv);

void freeResponse(Response* response);
