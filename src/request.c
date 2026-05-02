#include "../include/server.h"
#include <string.h>

void freeReq(Request* req){
    for(int i =0; i < req->nbHeaders; i+=1){

        free(req->headers[i].header);
        free(req->headers[i].value);
    }

    free(req->headers);
    free(req->route);
    free(req);
}


Request* parseRequest(char* buffer) {

    printf("[DEBUG] request: %s\n", buffer);
    Request* request = malloc(sizeof(Request));

    char* finMethod = strchr(buffer, ' ');

    if(finMethod == NULL) {
        free(request);
        return NULL;
    }


    char* stringMethod = malloc( finMethod-buffer+1 );
    strlcpy(stringMethod, buffer, finMethod-buffer+1);
    stringMethod[finMethod-buffer] = '\0';


    request->method = -1;
    if(strcmp(stringMethod, "GET") == 0) request->method = GET;
    if(strcmp(stringMethod, "POST") == 0) request->method = POST;

    free(stringMethod);

    if(request->method == -1) {
        free(request);
        return NULL;
    }


    char* endRoute = strstr(finMethod, "HTTP");
    if(endRoute == NULL) return NULL;

    // printf("[DEBUG] endROute: %s\n", endRoute);

    char* stringRoute = malloc(endRoute - 1 - finMethod - 1 + 1 );
    strlcpy(stringRoute, finMethod+1, endRoute - finMethod-1);
    stringRoute[ endRoute - finMethod-2] = '\0';

    request->route = stringRoute;
    // printf("[DEBUG] stringRoute: %s\n", stringRoute);

    int nbHeaders = 0;
    Header* headers = NULL;

    char* headerStart = strstr(buffer, "\r\n");
    if(headerStart == NULL) return NULL;

    while( headerStart != NULL && *(headerStart+2) != '\r') {
        printf("[DEBUG] heaedersStart: %s\n", headerStart);
        char* nextHeader = strstr(headerStart+2, "\r\n");
        if(nextHeader == NULL) {
            free(headers);
            return NULL;
        }

        nbHeaders+=1;
        if(nbHeaders == 0) headers = malloc(sizeof(Header));
        else headers = realloc(headers, nbHeaders*sizeof(Header));

        char* actualHeader = malloc(nextHeader-headerStart+1);
        strlcpy(actualHeader, headerStart, nextHeader-headerStart+1);
        actualHeader[nextHeader-headerStart] = '\0';

        // printf("[DEBUG] actualHeader: %s\n", actualHeader);

        char* delim = strchr(actualHeader, ':');
        char* headerDesc = malloc(delim-actualHeader+1);
        strlcpy(headerDesc, actualHeader, delim-actualHeader+1);
        headerDesc[delim-actualHeader] = '\0';

        // printf("[DEBUG] headerDesc: %s\n", headerDesc);

        char* headerValue = malloc(strlen(delim)-1);
        strlcpy(headerValue, delim+2, strlen(delim)-2);
        headerValue[strlen(delim)-2] = '\0';
        // printf("[DEBUG] headerValue: %s\n", headerValue);


        headers[nbHeaders-1].header = headerDesc;
        headers[nbHeaders-1].value = headerValue;

        free(actualHeader);




        headerStart = nextHeader;
    }

    printf("[DEBUG] lastHeader: %s\n", headerStart+3);
    char* body = malloc(strlen(headerStart+4));
    strcpy(body, headerStart+4);
    // printf("[DEBUG] body: %s ; len : %d\n", body, (int)strlen(body));

    request->headers = headers;
    request->nbHeaders = nbHeaders;
    request->body = body;

    return request;


}
