#include "../include/utils.h"
#include <stdlib.h>


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
