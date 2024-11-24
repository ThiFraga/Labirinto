#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Server.h"

#define MAX_SIZE 10 // Tamanho máximo da matriz
#define BUFSIZE 512 // Tamanho máximo do buffer


int identifyIPVersion(const char *vrs) {
    if(strcmp(vrs, "v4") == 0) return 1;
    else if (strcmp(vrs, "v6") == 0) return 2;
    else return 0;
}

int** getMazeFromFile(const char* filename, int* size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fclose(file);
        DieWithUserMessage("File not open","Please check if file path is correct");
        return NULL;
    }

    // Auxiliary variables to determine the size of the matrix
    char line[1024]; // Buffer to read a line
    int count = 0;

    // Determine the size of the matrix by counting the elements in the first line
    if (fgets(line, sizeof(line), file) != NULL) {
        char* token = strtok(line, " ");
        while (token != NULL) {
            count++;
            token = strtok(NULL, " ");
        }
    }

    // Restart the file to read the complete matrix
    rewind(file);

    // Validate the matrix size (must be 5 or 10)
    if (count != 5 && count != 10) {
        fprintf(stderr, "Error: Matrix invalid. Must be a 5x5 or 10x10 matrix.\n");
        fclose(file);
        return NULL;
    }

    *size = count;

    // Allocate memory for the matrix
    int** matrix = (int**)malloc(*size * sizeof(int*));
    for (int i = 0; i < *size; i++) {
        matrix[i] = (int*)malloc(*size * sizeof(int));
    }

    // Fill the matrix with values from the file
    for (int i = 0; i < *size; i++) {
        for (int j = 0; j < *size; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                fprintf(stderr, "Error on reading values of matrix.\n");
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return matrix;
}

void handleGame(int clntSocket,const char *filename) {
    char buffer[BUFSIZE]; // Buffer for mensagem recebida
    char response[BUFSIZE + 20]; // Buffer para resposta (com prefixo)
    int **maze;
    int mazeSize;
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE - 1, 0);
    if (numBytesRcvd < 0)
        DieWithSystemMessage("recv() failed");

    maze = getMazeFromFile(filename, &mazeSize);

    if (maze == NULL)
        DieWithUserMessage("Maze not found", "Unable to load maze from file");

    for(;;){

    }

    // Liberar memória alocada
    for (int i = 0; i < mazeSize; i++)
    {
        free(maze[i]);
    }
    free(maze);
}
