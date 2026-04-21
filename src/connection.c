#include "../include/connection.h"
#include <stdio.h>

void sendResponse(int new_socket, Response* reponse) {

    char* stringResponse = stringifyResponse(reponse);

    send(new_socket, stringResponse, strlen(stringResponse), 0);
    printf("Réponse envoyée au client.\n");
    freeResponse(reponse);
    free(stringResponse);

    close(new_socket); // Ferme la connexion avec le client actuel
}

Response* defaut(Response* reponse) {

    return( addContentToResponse("static/index.html", reponse) );

}

Response* staticRoute(char* route, Server* server, Response* reponse) {
    char* filepath = malloc(strlen(route)-strlen(server->staticRoute)+1);
    strlcpy(filepath, route+strlen(server->staticRoute) , strlen(route) - strlen(server->staticRoute)+1);
    filepath[ strlen(route) - strlen(server->staticRoute)] = '\0';



    char* filename = malloc(strlen(server->staticPath)+strlen(filepath)+2);
    sprintf(filename, "%s/%s", server->staticPath,filepath);


    reponse = addContentToResponse(filename, reponse);

    free(filepath);
    free(filename);

    return reponse;

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
        // printf("Message reçu du client : %s\n", buffer);
        Request* request = parseRequest(buffer);

        Response* response = malloc(sizeof(Response));

        // char *body = strdup("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
        char *body = strdup("");
        response->nbHeaders = 0;
        response->body = body;
        response->status = 200;
        response = addServerHeaderToResponse(server, response);

        Response* (*method)(Response*) = server->defautlMethod == NULL ? defaut : server->defautlMethod ;
        char customRoute = 0;

        if(server->staticRoute != NULL){

            char* verifStaticRoute = malloc(strlen(server->staticRoute)+1);

            strlcpy(verifStaticRoute, request->route, strlen(server->staticRoute)+1);
            verifStaticRoute[strlen(server->staticRoute)] = '\0';
            if(strcmp(verifStaticRoute, server->staticRoute)==0) {
                response = staticRoute(request->route, server, response);
                printf("[DEBUG] %s\n", response->body);
            }
            else customRoute = 1;
            free(verifStaticRoute);
        }

        else customRoute = 1;

        if(customRoute == 1){

            for(int i = 0; i < server->nbRoutes; i+=1) {
                if(strcmp(server->routes[i].stringRoute, request->route)==0) {
                    method = server->routes[i].method;
                    break;
                }
            }

            response = method(response);
        }


        sendResponse(new_socket, response);
        freeReq(request);

    }

    // char *reponse = strdup("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
    // reponse = addHeaderToResponse(server, reponse);
    // reponse = addContentToResponse("static/accueil.html", reponse);
    // // Envoi d'une réponse au client
    // send(new_socket, reponse, strlen(reponse), 0);
    // printf("Réponse envoyée au client.\n");
    // free(reponse);

    close(new_socket); // Ferme la connexion avec le client actuel


}
