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
    char* route;

}Request;

typedef struct Server {
    int port;
    struct sockaddr_in address;
    Header* headers;
    int nbHeaders;

    int server_fd;

}Server;

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
    int sz = ftell(fptr)-1;
    rewind(fptr);


    char myString[sz];
    fread(myString, sz, 1, fptr);

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

    char *reponse = strdup("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
    reponse = addHeaderToResponse(server, reponse);
    reponse = addContentToResponse("static/index.html", reponse);


    int valread = read(new_socket, buffer, BUFFER_SIZE - 1); // -1 pour garder de la place pour le '\0' terminal
    if (valread > 0) {
        printf("Message reçu du client : %s\n", buffer);
    }

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

    while(1) {
        handleConnection(&server);
    }

    close(server.server_fd);
    return 0;
}
