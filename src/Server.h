#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <string.h>

int identifyIPVersion(const char *vrs);
void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);
int** getMazeFromFile(const char* filename, int* size);

#endif