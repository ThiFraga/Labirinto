#ifndef CLIENT_H
#define CLIENT_H

typedef struct
{
    int type;
    int moves[100];
    int board[10][10];
} action;

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);
void cleanInput(char *str);
action getMessageType(char* msg);

#endif