#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

char* addContentToResponse(const char* filename, char* response);
void sendResponse(int new_socket, char* reponse);
