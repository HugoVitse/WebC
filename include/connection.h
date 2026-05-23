#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "../include/types.h"

#define BUFFER_SIZE 1024


typedef struct ThreadArgs {
    int socket;
    Server* server;
} ThreadArgs;

void handleConnection(Server* server);
