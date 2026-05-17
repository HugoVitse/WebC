#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>


Response* addFileContentToResponseBody(const char* filename, Response* response) {
    FILE *fptr;
    fptr = fopen(filename, "rb");
    printf("DEBUG : filename %s\n",filename);

    if (fptr == NULL) {
        fptr = fopen("defaultPages/notfound.html", "rb");
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

    response->body = realloc(response->body, strlen(response->body)+strlen(content)+1);
    strcat(response->body,content);

    return response;

}


ParsedBody* parseBody(char* body) {

    char* occur = strchr(body, '&');
    if(occur == NULL) {
        return NULL;
    }

    ParsedBody* parsedBody= malloc(sizeof(ParsedBody));
    int count = 1;

    while(occur != NULL) {
        occur = strchr(occur+1, '&');
        count+=1;
    }

    // printf("[DEBUG] countParams : %d\n",count);
    parsedBody->nbParams = count;
    parsedBody->params = malloc(count*sizeof(Parameter));

    char* start = body;
    char* end;
    char* delim;

    for(int i = 0; i < count; i+=1) {

        end = strchr(start, '&') == NULL ? body+strlen(body) :  strchr(start, '&');
        delim = strchr(start, '=');
        // printf("[DEBUG] start : %s, delim : %s, end : %s\n",start, delim, end);

        parsedBody->params[i].name = malloc(delim-start+1);
        strlcpy(parsedBody->params[i].name, start, delim-start+1);

        parsedBody->params[i].value = malloc(end-delim);
        strlcpy(parsedBody->params[i].value, delim+1, end-delim);


        // printf("[DEBUG] paramName : %s / paramValue : %s\n",parsedBody->params[i].name, parsedBody->params[i].value );


        start = end+1;
    }

    return parsedBody;

}

char* getValue(ParsedBody* body, char* value) {

    for(int i = 0; i < body->nbParams; i+=1) {

        if(strcmp(body->params[i].name, value) == 0) {
            return body->params[i].value;

        }

    }

    return NULL;
}
