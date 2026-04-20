#include "../include/request.h"

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

    Request* request = malloc(sizeof(Request));

    char* finMethod = strchr(buffer, ' ');
    char* stringMethod = malloc( finMethod-buffer+1 );
    strlcpy(stringMethod, buffer, finMethod-buffer+1);
    stringMethod[finMethod-buffer] = '\0';

    // printf("[DEBUG] stringMethod: %s\n", stringMethod);

    if(strcmp(stringMethod, "GET") == 0) request->method = GET;
    if(strcmp(stringMethod, "POST") == 0) request->method = POST;

    free(stringMethod);

    char* endRoute = strstr(finMethod, "HTTP");

    // printf("[DEBUG] endROute: %s\n", endRoute);

    char* stringRoute = malloc(endRoute - 1 - finMethod - 1 + 1 );
    strlcpy(stringRoute, finMethod+1, endRoute - finMethod-1);
    stringRoute[ endRoute - finMethod-2] = '\0';

    request->route = stringRoute;
    // printf("[DEBUG] stringRoute: %s\n", stringRoute);

    int nbHeaders = 0;
    Header* headers = NULL;

    char* headerStart = strstr(buffer, "\r\n");
    while( headerStart != NULL && *(headerStart+2) != '\r') {
        // printf("[DEBUG] heaedersStart: %s\n", headerStart);
        nbHeaders+=1;
        if(nbHeaders == 0) headers = malloc(sizeof(Header));
        else headers = realloc(headers, nbHeaders*sizeof(Header));


        char* nextHeader = strstr(headerStart+2, "\r\n");
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


    request->headers = headers;
    request->nbHeaders = nbHeaders;


    return request;


}
