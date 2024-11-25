#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int type;
    int moves[100];
    int board[10][10];
} Action;

typedef struct{
    int row;
    int col;
} PlayerPos;

int identifyIPVersion(const char *vrs);
void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);
int** getMazeFromFile(const char* filename, int* size);
void handleGame(int clntSocket,const char *filename);

#endif