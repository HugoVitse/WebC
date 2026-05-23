#include "../include/body.h"

#include <stdlib.h>
#include <string.h>


ParsedBody* parseBody(char* body) {

    ParsedBody* parsedBody= malloc(sizeof(ParsedBody));
    char* occur = strchr(body, '&');
    if(occur == NULL) {
        parsedBody->nbParams = 0;
        parsedBody->params = NULL;
        return parsedBody;
    }


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

    if(body == NULL) return "";

    for(int i = 0; i < body->nbParams; i+=1) {

        if(strcmp(body->params[i].name, value) == 0) {
            return body->params[i].value;

        }

    }

    return "";
}
