#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Server.h"

int main(int argc, char *argv[]) {
    FILE *arq;
    int **maze;
    int mazeSize;

    //check if the number of arguments is suficient
    if(argc < 5) DieWithUserMessage("Parameters","<IP Version> <Server Port> -i <maze file>");

    int ipParameter = identifyIPVersion(argv[1]); // check if is IPv4 IPv6 or invalid
    if(ipParameter == 0) DieWithUserMessage("Parameter"," <IP Version> invalid!");

    int domain;
    if(ipParameter == 1) domain = AF_INET;
    else if( ipParameter == 2) domain = AF_INET6;

    maze = getMazeFromFile(argv[4], &mazeSize);

    
    if (maze == NULL) {
        return 1; // Erro ao carregar a matriz
    }

    // Exibir a matriz carregada
    printf("Matriz %dx%d:\n", mazeSize, mazeSize);
    for (int i = 0; i < mazeSize; i++) {
        for (int j = 0; j < mazeSize; j++) {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }

    // Liberar memória alocada
    for (int i = 0; i < mazeSize; i++) {
        free(maze[i]);
    }
    free(maze);


    printf("fim");
    return 0;
}