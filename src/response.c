#include "../include/response.h"
#include "../include/header.h"
#include "../include/server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

char* stringifyResponse(Response* response, int* out_total_size) {

    char stringSize[32];
    sprintf(stringSize, "%d", response->bodyLen);
    addHeaderToResponse((Header){"Content-Length", stringSize}, response);


    size_t total_size = 17;

    for(int i = 0; i < response->nbHeaders; i++) {
        total_size += strlen(response->headers[i].header) + strlen(response->headers[i].value) + 4;
    }

    total_size += 2;
    total_size += response->bodyLen;

    char* stringified = malloc(total_size + 1);
    if (!stringified) return NULL;


    char* ptr = stringified;

    ptr += sprintf(ptr, "HTTP/1.1 %d OK\r\n", response->status);

    for(int i = 0; i < response->nbHeaders; i++) {
        ptr += sprintf(ptr, "%s: %s\r\n", response->headers[i].header, response->headers[i].value);
    }

    ptr += sprintf(ptr, "\r\n");

    if (response->bodyLen > 0 && response->body != NULL) {
        memcpy(ptr, response->body, response->bodyLen);
        ptr += response->bodyLen;
    }

    *ptr = '\0';

    if (out_total_size) {
        *out_total_size = (int)(ptr - stringified);
    }

    return stringified;
}


Response* addFileContentToResponseBody(const char* filename, Response* response) {
    FILE *fptr;
    fptr = fopen(filename, "rb");
    printf("DEBUG : filename %s\n",filename);

    if (fptr == NULL) {
        return (addContentToResponseBody("<!doctype html><html lang=\"en\">    <head>        <meta charset=\"UTF-8\" />        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />        <title>Document</title>    </head>    <body>        <h1>Not Found</h1>    </body></html>", response));
    }

    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);
    rewind(fptr);

    printf("DEBUG : size %d\n",sz);

    char *buffer = malloc(sz);
    if (buffer == NULL) {
        fclose(fptr);
        return response;
    }

    int bytesRead = fread(buffer, 1, sz, fptr);
    int current_len = response->bodyLen;

    printf("DEBUG : nbytesread %d , current : %d\n",bytesRead, current_len);


    response->body = realloc(response->body, current_len + bytesRead);
    memcpy(response->body + current_len, buffer, bytesRead);
    response->bodyLen = current_len + bytesRead;

    free(buffer);
    fclose(fptr);


    return response;
}


Response* addContentToResponseBody(const char* content, Response *response){

    response->body = realloc(response->body, response->bodyLen+strlen(content)+1);
    memcpy(response->body+response->bodyLen, content, strlen(content));

    response->bodyLen += strlen(content);
    return response;

}

void sendResponse(int new_socket, Response* reponse) {

    int total_bytes = 0;
    char* stringResponse = stringifyResponse(reponse, &total_bytes);
    send(new_socket, stringResponse, total_bytes, 0);
    printf("Réponse sent.\n");
    freeResponse(reponse);
    free(stringResponse);

    // close(new_socket); // ferme la connexion avec le client
}
