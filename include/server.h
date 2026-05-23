#pragma once
#include "types.h"

#include <arpa/inet.h>



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



void addGlobalHeader(Server* server, const char* header, const char* value);
void addRoute(Server* server, const char* route, Response* (*method)(Response*, Request*), enum METHOD verb);

void startServer(Server* server);
void freeServer(Server* serv);
void run(Server* serv);
void stop(Server* serv);
