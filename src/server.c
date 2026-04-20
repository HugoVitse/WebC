#include "../include/server.h"

void freeServer(Server* serv){

    for(int i =0; i < serv->nbHeaders; i+=1){
        free(serv->headers[i].header);
        free(serv->headers[i].value);
    }

}


void addRoute(Server* server, const char* route, char* (*method)(char*)){
    if(server->nbRoutes == 0) {
        server->routes = malloc(sizeof(Route));
    }
    else {
        server->routes = realloc(server->routes, (server->nbRoutes+1)*sizeof(Route));
    }

    server->routes[server->nbRoutes].stringRoute = malloc((strlen(route)+1));
    server->routes[server->nbRoutes].method  = method;

    strcpy(server->routes[server->nbRoutes].stringRoute, route);

    server->nbRoutes+=1;

}


void addHeader(Server* server, const char* header, const char* value){
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


char* addHeaderToResponse(Server* server, char* response){
    for(int i =0; i < server->nbHeaders;i+=1){

        size_t header_len = strlen(server->headers[i].header);
        size_t value_len = strlen(server->headers[i].value);

        size_t line_size = header_len + value_len + 5;

        char* headerString = malloc(line_size);
        sprintf(headerString, "%s: %s\r\n", server->headers[i].header, server->headers[i].value);


        response = realloc(response, ( strlen(response)+line_size));
        strcat(response, headerString);

        free(headerString);
    }
    return response;
}


void startServer(Server* server) {
    int opt = 1;
    server->nbHeaders = 0;
    server->staticRoute = NULL;

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
