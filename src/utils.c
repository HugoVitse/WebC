#include "../include/utils.h"


char* addContentToResponse(const char* filename, char* response) {
    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        response = realloc(response, strlen(response)+strlen("NOT FOUND")+1);
        strcat(response, "NOT FOUND");
        return response;
    }

    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);
    rewind(fptr);


    char myString[sz+1];
    fread(myString, sz, 1, fptr);
    myString[sz] = '\0';

    response = realloc(response, ( strlen(response)+sz+2));
    strcat(response, "\n");
    strcat(response, myString);


    fclose(fptr);


    return response;
}
