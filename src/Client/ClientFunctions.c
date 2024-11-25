#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Client.h"

void cleanInput(char *str) {
    char *end;

    // Remove espaços no início
    while (isspace((unsigned char)*str)) str++;

    // Remove espaços no final
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Adiciona o terminador nulo
    *(end + 1) = '\0';
}

action getMessageType(char *msg)
{
    action newMessage;
    for (int i = 0; i < 100; i++)
    {
        newMessage.moves[i] = 0;
    }

    cleanInput(msg);

    if (strcmp(msg, "up") == 0)
    {
        newMessage.type = 1;
        newMessage.moves[0] = 1;

    }
    else if (strcmp(msg, "right") == 0)
    {
        newMessage.type = 1;
        newMessage.moves[0] = 2;

    }
    else if (strcmp(msg, "down") == 0)
    {
        newMessage.type = 1;
        newMessage.moves[0] = 3;

    }
    else if (strcmp(msg, "left") == 0)
    {
        newMessage.type = 1;
        newMessage.moves[0] = 4;

    }
    else if (strcmp(msg, "map") == 0)
    {
        newMessage.type = 2;
    }
    else if (strcmp(msg, "start") == 0)
    {
        newMessage.type = 0;
    }
    else if (strcmp(msg, "reset") == 0)
    {
        newMessage.type = 6;
    }
    else if (strcmp(msg, "exit") == 0)
    {
        newMessage.type = 7;
    }
    else
    {
        newMessage.type = 10;
    }
    return newMessage;
}