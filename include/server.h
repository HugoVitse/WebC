#pragma once
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Header {
    char* header;
    char* value;
}Header;

typedef struct Route {
    char* stringRoute;
    char* (*method)(char*);
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

}Server;



void addHeader(Server* server, const char* header, const char* value);
void addRoute(Server* server, const char* route, char* (*method)(char*));
char* addHeaderToResponse(Server* server, char* response);
void startServer(Server* server);
void freeServer(Server* serv);
