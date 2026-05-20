#include "../include/connection.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void sendResponse(int new_socket, Response* reponse) {

    int total_bytes = 0;
    char* stringResponse = stringifyResponse(reponse, &total_bytes);
    send(new_socket, stringResponse, total_bytes, 0);
    printf("Réponse sent.\n");
    freeResponse(reponse);
    free(stringResponse);

    // close(new_socket); // ferme la connexion avec le client
}

Response* defaut(Response* reponse, Request* request) {

    return( addFileContentToResponseBody("defaultPages/index.html", reponse) );

}

Response* staticRoute(char* route, Server* server, Response* reponse) {

    char* filepath = malloc(strlen(route)-strlen(server->staticRoute)+1);
    strlcpy(filepath, route+strlen(server->staticRoute) , strlen(route) - strlen(server->staticRoute)+1);
    filepath[ strlen(route) - strlen(server->staticRoute)] = '\0';


    char* filename = malloc(strlen(server->staticPath)+strlen(filepath)+2);
    sprintf(filename, "%s/%s", server->staticPath,filepath);


    reponse = addFileContentToResponseBody(filename, reponse);

    free(filepath);
    free(filename);

    return reponse;

}

void* handleSocket(void* args) {

    ThreadArgs* castedArgs = (ThreadArgs*)args;
    printf("[DEBUG] Un client s'est connecté \n");

    char buffer[BUFFER_SIZE] = {0};

    int closeConnection = 0;
    int valread; // -1 pour garder de la place pour le '\0' terminal
    int countRequest = 0;

    int timeout = 10;

    time_t init = time(NULL);
    time_t actual = time(NULL);

    while(closeConnection != 1) {
        valread = read(castedArgs->socket, buffer, BUFFER_SIZE - 1);

        if (valread > 0) {
            buffer[valread] = '\0';
            printf("Message reçu du client : %s\n", buffer);
            countRequest+=1;
            //
            // printf("[DEBUG] Nombre de requete dans le meme socket : %d\n", countRequest);
            Request* request = parseRequest(buffer);
            Response* response = malloc(sizeof(Response));


            // char *body = strdup("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
            char *body = strdup("");
            response->nbHeaders = 0;
            response->body = body;
            response->bodyLen = 0;
            response->status = 200;
            response = addServerHeaderToResponse(castedArgs->server, response);
            if(request==NULL) {
                response->status = 400;
                addHeaderToResponse( (Header){"Content-Type", "text/html"} , response);
                addFileContentToResponseBody("defaultPages/badrequest.html", response);
                sendResponse(castedArgs->socket, response);
                continue;
            }


            Response* (*method)(Response*, Request*) = castedArgs->server->defautlMethod == NULL ? defaut : castedArgs->server->defautlMethod ;
            char customRoute = 0;

            if(castedArgs->server->staticRoute != NULL){

                char* verifStaticRoute = malloc(strlen(castedArgs->server->staticRoute)+1);

                strlcpy(verifStaticRoute, request->route, strlen(castedArgs->server->staticRoute)+1);
                verifStaticRoute[strlen(castedArgs->server->staticRoute)] = '\0';
                if(strcmp(verifStaticRoute, castedArgs->server->staticRoute)==0) {
                    response = staticRoute(request->route, castedArgs->server, response);
                    printf("[DEBUG] %s\n", response->body);
                }
                else customRoute = 1;
                free(verifStaticRoute);
            }


            else customRoute = 1;

            if(customRoute == 1){

                for(int i = 0; i < castedArgs->server->nbRoutes; i+=1) {
                    if(strcmp(castedArgs->server->routes[i].stringRoute, request->route)==0 && request->method == castedArgs->server->routes[i].verb) {
                        method = castedArgs->server->routes[i].method;
                        break;
                    }
                }

                response = method(response, request);
            }


            sendResponse(castedArgs->socket, response);
            freeReq(request);

        }
        actual = time(NULL);

        if(actual - init >=timeout ) {
            closeConnection = 1;
        }
    }


    printf("[DEBUG] Timeout connection close\n");

    close(castedArgs->socket);
    free(castedArgs);
    pthread_exit(NULL);

}

void handleConnection(Server* server){

    printf("webc en attente sur le port %d...\n", server->port);

    int new_socket;
    socklen_t addrlen = sizeof(server->address);

    if ((new_socket = accept(server->server_fd, (struct sockaddr *)&server->address, &addrlen)) < 0) {
        perror("Échec de accept");
        exit(EXIT_FAILURE);
    }
    pthread_t mon_thread;

    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args->socket = new_socket;
    args->server = server;

    if (pthread_create(&mon_thread, NULL, handleSocket, args) != 0) {
        perror("Erreur lors de la création du thread");
        free(args);
        close(new_socket);
    }else {
        pthread_detach(mon_thread);
    }


}
