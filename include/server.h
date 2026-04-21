#pragma once
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

typedef struct Response{

    Header* headers;
    int nbHeaders;

    int status;
    char* body;

} Response;

typedef struct Route {
    char* stringRoute;
    Response* (*method)(Response*);
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

    Response* (*defautlMethod)(Response*);

}Server;





void addGlobalHeader(Server* server, const char* header, const char* value);
void addRoute(Server* server, const char* route, Response* (*method)(Response*));
Response* addServerHeaderToResponse(Server* server, Response* response);
Response* addHeaderToResponse(Header header, Response* response);
char* stringifyResponse(Response* response);

void startServer(Server* server);
void freeServer(Server* serv);
void run(Server* serv);
void stop(Server* serv);

void freeResponse(Response* response);
