#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Client.h"

#define MAXMOVENAMESIZE 10 // Tamanho máximo de um nome de movimento

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

action getMessage(char *msg)
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


void getMoveName(char *move, int value) {
    switch (value)
    {
    case 1:
        strcpy(move, "up");
        break;
    case 2:
        strcpy(move, "right");
        break;
    case 3:
        strcpy(move, "down");
        break;
    case 4:
        strcpy(move, "left");
        break;
    default:
        strcpy(move, "");
        break;
    }
}

void printPossibleMoves(int moves[]) {
    int i = 0;
    char moveName[MAXMOVENAMESIZE];

    fputs("Possible moves: ", stdout);

    while (moves[i] != 0) { // Verifica se ainda há movimentos
        getMoveName(moveName, moves[i]); // Obtém o nome do movimento
        if (i != 0) fputs(", ", stdout); // Adiciona vírgula após o primeiro movimento
        fputs(moveName, stdout); // Imprime o nome do movimento
        i++;
    }
    fputs(".\n", stdout);
}

char* getCharEquivalent(int value) {
    char *res;
    switch (value)
    {
    case 0:
        res = "#";
        break;
    case 1:
        res = "_";
        break;
    case 2:
        res = ">";
        break;
    case 3:
        res = "X";
        break;
    case 4:
        res = "?";
        break;
    case 5:
        res = "+";
        break;
    
    default:
        res = "";
        break;
    }
    return res;
}

void printMap(int maze[MAXMAZESIZE][MAXMAZESIZE]) {
    for(int i = 0; i < MAXMAZESIZE; i++) {
        for(int j = 0; j < MAXMAZESIZE; j++){
            printf("%s ",getCharEquivalent(maze[i][j]));
        }
        printf("\n");
    }
    printf("\n");
}
