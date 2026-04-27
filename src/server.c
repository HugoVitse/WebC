#include "../include/server.h"
#include <string.h>

void freeServer(Server* serv){

    for(int i =0; i < serv->nbHeaders; i+=1){
        free(serv->headers[i].header);
        free(serv->headers[i].value);
    }

}


void addRoute(Server* server, const char* route, Response* (*method)(Response*, Request*), enum METHOD verb){
    if(server->nbRoutes == 0) {
        server->routes = malloc(sizeof(Route));
    }
    else {
        server->routes = realloc(server->routes, (server->nbRoutes+1)*sizeof(Route));
    }

    server->routes[server->nbRoutes].stringRoute = malloc((strlen(route)+1));
    server->routes[server->nbRoutes].method  = method;
    server->routes[server->nbRoutes].verb = verb;

    strcpy(server->routes[server->nbRoutes].stringRoute, route);

    server->nbRoutes+=1;

}


void addGlobalHeader(Server* server, const char* header, const char* value){
    if(server->nbHeaders == 0) {
        server->headers = malloc(sizeof(Header));
    }
    else {
        server->headers = realloc(server->headers, (server->nbHeaders+1)*sizeof(Header));
    }

    server->headers[server->nbHeaders].header = malloc((strlen(header)+1)*sizeof(char));
    server->headers[server->nbHeaders].value = malloc((strlen(value)+1)*sizeof(char));

    strcpy(server->headers[server->nbHeaders].header, header);
    strcpy(server->headers[server->nbHeaders].value, value);

    server->nbHeaders+=1;

}

Response* addServerHeaderToResponse(Server* server, Response* response) {
    for(int i=0; i< server->nbHeaders; i+=1) {
        response = addHeaderToResponse(server->headers[i], response);
    }
    return response;
}

Response* addHeaderToResponse(Header header, Response* response){
    response->nbHeaders+=1;
    if(response->nbHeaders == 1) response->headers = malloc(sizeof(Header));
    else response->headers = realloc(response->headers, response->nbHeaders*sizeof(Header));

    response->headers[response->nbHeaders-1] = header;
    return response;
}

void freeResponse(Response* response) {

    free(response->body);
    free(response->headers);
    free(response);

}

void startServer(Server* server) {
    int opt = 1;
    server->nbHeaders = 0;
    server->staticRoute = NULL;
    server->defautlMethod = NULL;
    server->nbRoutes = 0;
    server->routes = NULL;

    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Échec de la création du socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt a échoué");
        exit(EXIT_FAILURE);
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces réseau (0.0.0.0)
    server->address.sin_port = htons(server->port);

    if (bind(server->server_fd, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        perror("Échec du bind");
        exit(EXIT_FAILURE);
    }

}

void handleConnection(Server *server);

void run(Server* serv) {
    if (listen(serv->server_fd, 3) < 0) {
        perror("Échec du listen");
        exit(EXIT_FAILURE);
    }
    while(1) {
        handleConnection(serv);
    }
}

void stop(Server* serv){

    freeServer(serv);
    close(serv->server_fd);
}


char* stringifyResponse(Response* response) {

    int div = 10;
    int size = 1;
    int bodySize = strlen(response->body);
    while(bodySize/div > 0) {
        div*=10;
        size+=1;
    }

    size+=2;
    char* stringSize = malloc(size);
    sprintf(stringSize, "%d", bodySize);
    printf("[DEBUG] stringsize : %s, realsize : %d\n", stringSize, bodySize);
    addHeaderToResponse((Header){"Content-Length", stringSize},response);

    char* stringified = malloc(18);
    sprintf(stringified, "HTTP/1.1 %d OK\r\n", response->status);

    // printf("[DEBUG] body : %s\n",response->body);
    // printf("[DEBUG] stringified : %s\n",stringified);


    for(int i =0; i < response->nbHeaders;i+=1){

        size_t header_len = strlen(response->headers[i].header);
        size_t value_len = strlen(response->headers[i].value);

        size_t line_size = header_len + value_len + 5;

        char* headerString = malloc(line_size);
        sprintf(headerString, "%s: %s\r\n", response->headers[i].header, response->headers[i].value);


        stringified = realloc(stringified, ( strlen(stringified)+line_size));
        strcat(stringified, headerString);

        free(headerString);
        // printf("[DEBUG] stringified : %s\n",stringified);

    }

    stringified = realloc(stringified, ( strlen(stringified)+strlen(response->body)+3));
    strcat(stringified, "\r\n");

    strcat(stringified, response->body);
    // printf("[DEBUG] stringified : %s\n",stringified);
    //


    return stringified;



}
