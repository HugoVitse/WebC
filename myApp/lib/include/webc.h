#pragma once

typedef struct Request Request;
typedef struct Response Response;
typedef struct Header Header;
typedef struct ThreadArgs ThreadArgs;

typedef struct Parameter Parameter;
typedef struct ParsedBody ParsedBody;

typedef struct Server Server;
typedef struct Route Route;

enum METHOD {
    GET,
    POST,
    PUT,
    HEAD,
    OPTIONS,
    DELETE
};

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


#define BUFFER_SIZE 1024


typedef struct ThreadArgs {
    int socket;
    Server* server;
} ThreadArgs;

void handleConnection(Server* server);


typedef struct Request {

    enum METHOD method;
    Header* headers;
    int nbHeaders;
    char* route;
    char* body;

}Request;


void freeReq(Request* req);
Request* parseRequest(char* buffer);


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

 typedef struct Header {
     char* header;
     char* value;
 }Header;
