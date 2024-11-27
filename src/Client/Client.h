#ifndef CLIENT_H
#define CLIENT_H

#define MAXMOVES 100
#define MAXMAZESIZE 10
#define MAXMOVENAMESIZE 10

typedef struct
{
    int type;
    int moves[100];
    int board[10][10];
} action;

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);
void cleanInput(char *str);
action getMessage(char *msg);
void printPossibleMoves(int moves[]);
char *getCharEquivalent(int value);
void printMap(int maze[MAXMAZESIZE][MAXMAZESIZE]);
int isValidMovement(int movement, int possibleMoves[MAXMOVES]);
void printHint(int moves[]);

#endif