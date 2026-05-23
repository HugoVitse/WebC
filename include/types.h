#pragma once

typedef struct Request Request;
typedef struct Response Response;
typedef struct Header Header;
typedef struct ThreadArgs ThreadArgs;

typedef struct Parameter Parameter;
typedef struct ParsedBody ParsedBody;

typedef struct Server Server;
typedef struct Route Route;

enum METHOD {
    GET,
    POST,
    PUT,
    HEAD,
    OPTIONS,
    DELETE
};
