#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define MAXMOVES 100
#define MAXMAZESIZE 10
#define MAXMOVENAMESIZE 10

typedef struct
{
    int type;
    int moves[MAXMOVES];
    int board[MAXMAZESIZE][MAXMAZESIZE];
} Action;

typedef struct
{
    int row;
    int col;
} PlayerPos;

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);

#endif