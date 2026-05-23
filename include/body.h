#pragma once
#include "types.h"


typedef struct Parameter {
    char* name;
    char* value;
} Parameter;

typedef struct ParsedBody{
    Parameter* params;
    int nbParams;
} ParsedBody;

ParsedBody* parseBody(char* body);
char* getValue(ParsedBody* body, char* value);
