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

int identifyIPVersion(const char *vrs)
{
    if (strcmp(vrs, "v4") == 0)
        return 1;
    else if (strcmp(vrs, "v6") == 0)
        return 2;
    else
        return 0;
}

int **getMazeFromFile(const char *filename, int *size)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fclose(file);
        DieWithUserMessage("File not open", "Please check if file path is correct");
        return NULL;
    }

    // Auxiliary variables to determine the size of the matrix
    char line[1024]; // Buffer to read a line
    int count = 0;

    // Determine the size of the matrix by counting the elements in the first line
    if (fgets(line, sizeof(line), file) != NULL)
    {
        char *token = strtok(line, " ");
        while (token != NULL)
        {
            count++;
            token = strtok(NULL, " ");
        }
    }

    // Restart the file to read the complete matrix
    rewind(file);

    // Validate the matrix size (must be 5 or 10)
    if (count != 5 && count != 10)
    {
        fprintf(stderr, "Error: Matrix invalid. Must be a 5x5 or 10x10 matrix.\n");
        fclose(file);
        return NULL;
    }

    *size = count;

    // Allocate memory for the matrix
    int **matrix = (int **)malloc(*size * sizeof(int *));
    for (int i = 0; i < *size; i++)
    {
        matrix[i] = (int *)malloc(*size * sizeof(int));
    }

    // Fill the matrix with values from the file
    for (int i = 0; i < *size; i++)
    {
        for (int j = 0; j < *size; j++)
        {
            if (fscanf(file, "%d", &matrix[i][j]) != 1)
            {
                fprintf(stderr, "Error on reading values of matrix.\n");
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return matrix;
}

int **getInitialUpdatedMaze(int **mazeInitial, int mazeSize, PlayerPos *pos, PlayerPos *exit)
{
    int i, j;

    // Alocação dinâmica para nova matriz
    int **newMaze = (int **)malloc(mazeSize * sizeof(int *));
    for (i = 0; i < mazeSize; i++)
    {
        newMaze[i] = (int *)malloc(mazeSize * sizeof(int));
    }

    // Encontrar a posição inicial do jogador (valor 2 na matriz)
    for (i = 0; i < mazeSize; i++)
    {
        for (j = 0; j < mazeSize; j++)
        {
            if (mazeInitial[i][j] == 2)
            {
                pos->row = i;
                pos->col = j;
            }
        }
    }

    // Atualizar a nova matriz com base na posição inicial do jogador
    for (i = 0; i < mazeSize; i++)
    {
        for (j = 0; j < mazeSize; j++)
        {
            int iDist = abs(pos->row - i);
            int jDist = abs(pos->col - j);

            if (iDist <= 1 && jDist <= 1)
            {
                // Mantém valores próximos ao jogador
                newMaze[i][j] = mazeInitial[i][j];
            }
            else
            {
                // Áreas desconhecidas marcadas como 4
                newMaze[i][j] = 4;
            }
            if (mazeInitial[i][j] == 3)
            {
                exit->row = i;
                exit->col = j;
            }
        }
    }
    newMaze[pos->row][pos->col] = 5;
    return newMaze;
}

int isFreePath(int value)
{
    int res = 0;

    switch (value)
    {
    case 1:
    case 2:
    case 3:
        res = 1;
        break;

    default:
        res = 0;
        break;
    }
    return res;
}

void getPossibleMoves(int **maze, PlayerPos pos, int moves[MAXMOVES], int mazeSize)
{
    int i = 0;
    while (i < MAXMOVES)
    {
        moves[i] = 0;
        i++;
    }
    i = 0;
    if (pos.row > 0 && isFreePath(maze[pos.row - 1][pos.col]) == 1)
    {
        moves[i] = 1;
        i++;
    }
    if (pos.col < mazeSize - 1 && isFreePath(maze[pos.row][pos.col + 1]) == 1)
    {
        moves[i] = 2;
        i++;
    }
    if (pos.row < mazeSize - 1 && isFreePath(maze[pos.row + 1][pos.col]) == 1)
    {
        moves[i] = 3;
        i++;
    }
    if (pos.col > 0 && isFreePath(maze[pos.row][pos.col - 1]) == 1)
    {
        moves[i] = 4;
        i++;
    }
}

void copyMap(int **original, int copy[MAXMAZESIZE][MAXMAZESIZE], int mazeSize)
{
    for (int i = 0; i < MAXMAZESIZE; i++)
    {
        for (int j = 0; j < MAXMAZESIZE; j++)
        {
            if (i < mazeSize && j < mazeSize)
                copy[i][j] = original[i][j];
            else
                copy[i][j] = 9;
        }
    }
}

int updateGame(int **updatedMaze, int **initialMaze, PlayerPos *pos, PlayerPos exit, int movement, int mazeSize)
{
    updatedMaze[pos->row][pos->col] = initialMaze[pos->row][pos->col];

    switch (movement)
    {
    case 1:
        pos->row--;
        break;
    case 2:
        pos->col++;
        break;
    case 3:
        pos->row++;
        break;
    case 4:
        pos->col--;
        break;

    default:
        break;
    }
    // update player position
    updatedMaze[pos->row][pos->col] = 5;

    // update maze
    if (pos->row > 0)
        updatedMaze[pos->row - 1][pos->col] = initialMaze[pos->row - 1][pos->col];
    if (pos->row > 0 && pos->col > 0)
        updatedMaze[pos->row - 1][pos->col - 1] = initialMaze[pos->row - 1][pos->col - 1];
    if (pos->col > 0)
        updatedMaze[pos->row][pos->col - 1] = initialMaze[pos->row][pos->col - 1];
    if (pos->col > 0 && pos->row < mazeSize - 1)
        updatedMaze[pos->row + 1][pos->col - 1] = initialMaze[pos->row + 1][pos->col - 1];
    if (pos->row < mazeSize - 1)
        updatedMaze[pos->row + 1][pos->col] = initialMaze[pos->row + 1][pos->col];
    if (pos->col < mazeSize - 1 && pos->row < mazeSize - 1)
        updatedMaze[pos->row + 1][pos->col + 1] = initialMaze[pos->row + 1][pos->col + 1];
    if (pos->col < mazeSize - 1)
        updatedMaze[pos->row][pos->col + 1] = initialMaze[pos->row][pos->col + 1];
    if (pos->row > 0 && pos->col < mazeSize - 1)
        updatedMaze[pos->row - 1][pos->col + 1] = initialMaze[pos->row - 1][pos->col + 1];

    if (pos->row == exit.row && pos->col == exit.col)
        return 1;
    return 0;
}

void handleGame(int clntSocket, const char *filename)
{
    char buffer[BUFSIZE];        // Buffer for mensagem recebida
    char response[BUFSIZE + 20]; // Buffer para resposta (com prefixo)
    Action msgRecv, msgToSend;
    int **mazeInitial, **mazeUpdated, mazeSize, hasStarted = 0, hasExited = 0;
    ssize_t numBytesRcvd, numBytesSent;
    PlayerPos player, exit;

    for (;;)
    {
        numBytesRcvd = recv(clntSocket, &msgRecv, sizeof(msgRecv), 0);

        if (numBytesRcvd >= 0)
        {
            switch (msgRecv.type)
            {
            case 0: // handle command "start"
                if (hasStarted > 0)
                    break;
                hasStarted = 1;
                mazeInitial = getMazeFromFile(filename, &mazeSize);
                if (mazeInitial == NULL)
                    DieWithUserMessage("Maze not found", "Unable to load maze from file");

            case 6: // handle command "reset" and final part of command "start"
                mazeUpdated = getInitialUpdatedMaze(mazeInitial, mazeSize, &player, &exit);
                printf("starting new game\n");
                msgToSend.type = 4;
                getPossibleMoves(mazeUpdated, player, msgToSend.moves, mazeSize);

                numBytesSent = send(clntSocket, &msgToSend, sizeof(msgToSend), 0);
                break;

            case 1: // handle command "move"
                if (hasStarted == 0)
                    break;

                if (updateGame(mazeUpdated, mazeInitial, &player, exit, msgRecv.moves[0], mazeSize) != 1)
                {
                    msgToSend.type = 4;
                    getPossibleMoves(mazeUpdated, player, msgToSend.moves, mazeSize);
                }
                else
                {
                    msgToSend.type = 5;
                    copyMap(mazeInitial, msgToSend.board, mazeSize);
                }

                numBytesSent = send(clntSocket, &msgToSend, sizeof(msgToSend), 0);
                break;

            case 2: // handle command "map"
                msgToSend.type = 4;
                copyMap(mazeUpdated, msgToSend.board, mazeSize);
                numBytesSent = send(clntSocket, &msgToSend, sizeof(msgToSend), 0);
                break;
            
            case 7:
                hasExited = 1;
                break;
             
            default:
                break;
            }
        }
        if(hasExited == 1) break;
    }
    // Liberar memória alocada
    for (int i = 0; i < mazeSize; i++)
    {
        free(mazeInitial[i]);
        free(mazeUpdated[i]);
    }
    free(mazeInitial);
    free(mazeUpdated);

}
