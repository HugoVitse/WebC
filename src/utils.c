#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>


Response* addFileContentToResponseBody(const char* filename, Response* response) {
    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        fptr = fopen("static/notfound.html", "r");
    }

    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);
    rewind(fptr);


    char myString[sz+1];
    fread(myString, sz, 1, fptr);
    myString[sz] = '\0';

    response->body = realloc(response->body, ( strlen(response->body)+sz+2));
    strcat(response->body, "\n");
    strcat(response->body, myString);


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
