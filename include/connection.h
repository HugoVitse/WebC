#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../include/server.h"
#include "../include/request.h"
#include "../include/utils.h"

#define BUFFER_SIZE 1024

void handleConnection(Server* server);
