#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

enum METHOD {
    GET,
    POST
};



typedef struct Header {
    char* header;
    char* value;
}Header;

typedef struct Request {

    enum METHOD method;
    Header* headers;
    int nbHeaders;
    char* route;

}Request;

typedef struct Route {
    char* stringRoute;
    char* (*method)(char*);
}Route;

typedef struct Server {
    int port;
    struct sockaddr_in address;
    Header* headers;
    int nbHeaders;

    int server_fd;

    Route* routes;
    int nbRoutes;

}Server;


void freeReq(Request* req){
    for(int i =0; i < req->nbHeaders; i+=1){
        free(req->headers[i].header);
        free(req->headers[i].value);
    }
    free(req->headers);
    free(req->route);
    free(req);
}

void freeServer(Server* serv){

    for(int i =0; i < serv->nbHeaders; i+=1){
        free(serv->headers[i].header);
        free(serv->headers[i].value);
    }

}

Request* parseRequest(char* buffer) {

    Request* request = malloc(sizeof(Request));

    char* finMethod = strchr(buffer, ' ');
    char* stringMethod = malloc( finMethod-buffer+1 );
    strlcpy(stringMethod, buffer, finMethod-buffer+1);
    stringMethod[finMethod-buffer] = '\0';

    printf("[DEBUG] stringMethod: %s\n", stringMethod);

    if(strcmp(stringMethod, "GET") == 0) request->method = GET;
    if(strcmp(stringMethod, "POST") == 0) request->method = POST;

    free(stringMethod);

    char* endRoute = strstr(finMethod, "HTTP");

    printf("[DEBUG] endROute: %s\n", endRoute);

    char* stringRoute = malloc(endRoute - 1 - finMethod - 1 + 1 );
    strlcpy(stringRoute, finMethod+1, endRoute - finMethod-1);
    stringRoute[ endRoute - finMethod-2] = '\0';

    request->route = stringRoute;
    printf("[DEBUG] stringRoute: %s\n", stringRoute);

    int nbHeaders = 0;
    Header* headers = NULL;

    char* headerStart = strstr(buffer, "\r\n");
    while( headerStart != NULL && *(headerStart+2) != '\r') {
        printf("[DEBUG] heaedersStart: %s\n", headerStart);
        nbHeaders+=1;
        if(nbHeaders == 0) headers = malloc(sizeof(Header));
        else headers = realloc(headers, nbHeaders*sizeof(Header));


        char* nextHeader = strstr(headerStart+2, "\r\n");
        char* actualHeader = malloc(nextHeader-headerStart+1);
        strlcpy(actualHeader, headerStart, nextHeader-headerStart+1);
        actualHeader[nextHeader-headerStart] = '\0';

        printf("[DEBUG] actualHeader: %s\n", actualHeader);

        char* delim = strchr(actualHeader, ':');
        char* headerDesc = malloc(delim-actualHeader+1);
        strlcpy(headerDesc, actualHeader, delim-actualHeader+1);
        headerDesc[delim-actualHeader] = '\0';

        printf("[DEBUG] headerDesc: %s\n", headerDesc);

        char* headerValue = malloc(strlen(delim)-1);
        strlcpy(headerValue, delim+2, strlen(delim)-2);
        headerValue[strlen(delim)-2] = '\0';
        printf("[DEBUG] headerValue: %s\n", headerValue);


        headers[nbHeaders-1].header = headerDesc;
        headers[nbHeaders-1].value = headerValue;

        free(actualHeader);




        headerStart = nextHeader;
    }


    request->headers = headers;
    request->nbHeaders = nbHeaders;


    return request;


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

char* addContentToResponse(const char* filename, char* response) {
    FILE *fptr;
    fptr = fopen(filename, "r");

    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);
    rewind(fptr);


    char myString[sz+1];
    fread(myString, sz, 1, fptr);
    myString[sz] = '\0';
    printf("string  : %s : %d\n", myString, sz);

    response = realloc(response, ( strlen(response)+sz+2));
    strcat(response, "\n");
    strcat(response, myString);


    fclose(fptr);


    return response;
}


void startServer(Server* server) {
    int opt = 1;
    server->nbHeaders = 0;

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

void sendResponse(int new_socket, char* reponse) {

    send(new_socket, reponse, strlen(reponse), 0);
    printf("Réponse envoyée au client.\n");
    free(reponse);

    close(new_socket); // Ferme la connexion avec le client actuel
}

char* connard(char* reponse){

    return( addContentToResponse("static/accueil.html", reponse) );


}

char* defaut(char* reponse) {

    return( addContentToResponse("static/index.html", reponse) );

}


void handleConnection(Server* server){
    if (listen(server->server_fd, 3) < 0) {
        perror("Échec du listen");
        exit(EXIT_FAILURE);
    }

    printf("webc en attente sur le port %d...\n", server->port);

    int new_socket;
    socklen_t addrlen = sizeof(server->address);

    if ((new_socket = accept(server->server_fd, (struct sockaddr *)&server->address, &addrlen)) < 0) {
        perror("Échec de accept");
        exit(EXIT_FAILURE);
    }

    printf("[DEBUG] Un client s'est connecté \n");

    char buffer[BUFFER_SIZE] = {0};


    int valread = read(new_socket, buffer, BUFFER_SIZE - 1); // -1 pour garder de la place pour le '\0' terminal
    if (valread > 0) {
        printf("Message reçu du client : %s\n", buffer);
        Request* request = parseRequest(buffer);
        char *reponse = strdup("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
        reponse = addHeaderToResponse(server, reponse);

        char* (*method)(char*) = defaut;

        for(int i = 0; i < server->nbRoutes; i+=1) {
            printf("[DEBUG] i : %d ; %s\n", i,server->routes[i].stringRoute);
            if(strcmp(server->routes[i].stringRoute, request->route)==0) {
                method = server->routes[i].method;
                break;
            }
        }

        reponse = method(reponse);

        sendResponse(new_socket, reponse);
        freeReq(request);

    }

    char *reponse = strdup("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
    reponse = addHeaderToResponse(server, reponse);
    reponse = addContentToResponse("static/accueil.html", reponse);
    // Envoi d'une réponse au client
    send(new_socket, reponse, strlen(reponse), 0);
    printf("Réponse envoyée au client.\n");
    free(reponse);

    close(new_socket); // Ferme la connexion avec le client actuel


}

int main() {

    Server server;
    server.port = 8080;

    startServer(&server);
    addHeader(&server, "Server" , "WebC");
    addRoute(&server, "/test", connard);

    while(1) {
        handleConnection(&server);
    }

    freeServer(&server);
    close(server.server_fd);
    return 0;
}
