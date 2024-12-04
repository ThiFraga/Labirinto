#include "Common.h"

/* Summary
    Server Functions line 8
    Main Functions line 206
*/

// Server Functions

// Global variable for moves on hint
int dx[4] = {0, 1, 0, -1};
int dy[4] = {-1, 0, 1, 0};

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

    int **newMaze = (int **)malloc(mazeSize * sizeof(int *));
    for (i = 0; i < mazeSize; i++)
    {
        newMaze[i] = (int *)malloc(mazeSize * sizeof(int));
    }

    // Find player initial position
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

    // update matrix arround player poisition
    for (i = 0; i < mazeSize; i++)
    {
        for (j = 0; j < mazeSize; j++)
        {
            int iDist = abs(pos->row - i);
            int jDist = abs(pos->col - j);

            if (iDist <= 1 && jDist <= 1)
            {
                newMaze[i][j] = mazeInitial[i][j];
            }
            else
            {
                // Unknown areas
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

// Depth-firs Search algorithm
int dfs(int **maze, int **visited, int mazeSize, PlayerPos pos, PlayerPos exit, int path[], int *index)
{
    PlayerPos newPos; // new player position

    if (pos.row == exit.row && pos.col == exit.col)
        return 1; // if its the exit return 1
    if (*index >= MAXMOVES)
        return 0; // return 0 if exit was not found

    visited[pos.row][pos.col] = -1;

    for (int i = 0; i < 4; i++) // for each possible movement
    {
        newPos.row = pos.row + dy[i];
        newPos.col = pos.col + dx[i];

        if (newPos.row >= 0 && newPos.row < mazeSize && newPos.col >= 0 && newPos.col < mazeSize &&
            maze[newPos.row][newPos.col] != 0 && visited[newPos.row][newPos.col] == 0)
        {

            path[*index] = i + 1; // Save movement
            (*index)++;

            if (dfs(maze, visited, mazeSize, newPos, exit, path, index) == 1)
                return 1;

            (*index)--; // Go back if has no solution
        }
    }

    visited[pos.row][pos.col] = 0; // Restart position for new attempts
    return 0;
}

void getHintMoves(int **maze, int moves[], int mazeSize, PlayerPos pos, PlayerPos exit)
{
    int path[MAXMOVES] = {0};
    int **visited;
    int index = 0;

    visited = (int **)malloc(mazeSize * sizeof(int *));
    for (int i = 0; i < mazeSize; i++)
    {
        visited[i] = (int *)malloc(mazeSize * sizeof(int));
    }

    // Initialize the matrix `visited` with zeros
    for (int i = 0; i < mazeSize; i++)
    {
        for (int j = 0; j < mazeSize; j++)
        {
            visited[i][j] = 0;
        }
    }

    // Call the DFS to find the path
    dfs(maze, visited, mazeSize, pos, exit, path, &index);

    // Copy values to moves vector
    for (int i = 0; i < index; i++)
    {
        moves[i] = path[i];
    }

    for (int i = 0; i < mazeSize; i++)
    {
        free(visited[i]);
    }
    free(visited);
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
                if (mazeInitial == NULL){
                    DieWithUserMessage("Maze not found", "Unable to load maze from file");
                }
                // fall through
            case 6: // handle command "reset" and final part of command "start"
                mazeUpdated = getInitialUpdatedMaze(mazeInitial, mazeSize, &player, &exit);
                printf("starting new game\n");
                msgToSend.type = 4;
                getPossibleMoves(mazeUpdated, player, msgToSend.moves, mazeSize);

                numBytesSent = send(clntSocket, &msgToSend, sizeof(msgToSend), 0);
                if (numBytesSent < 0)
                    DieWithSystemMessage("send() failed");
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
                if (numBytesSent < 0)
                    DieWithSystemMessage("send() failed");
                break;

            case 2: // handle command "map"
                msgToSend.type = 4;
                copyMap(mazeUpdated, msgToSend.board, mazeSize);
                numBytesSent = send(clntSocket, &msgToSend, sizeof(msgToSend), 0);
                if (numBytesSent < 0)
                    DieWithSystemMessage("send() failed");
                break;
            case 3: // handle command "hint"
                msgToSend.type = 4;
                getHintMoves(mazeInitial, msgToSend.moves, mazeSize, player, exit);
                numBytesSent = send(clntSocket, &msgToSend, sizeof(msgToSend), 0);
                if (numBytesSent < 0)
                    DieWithSystemMessage("send() failed");
                break;

            case 7:
                hasExited = 1;
                break;

            default:
                break;
            }
        }
        if (hasExited == 1)
            break;
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

int main(int argc, char *argv[])
{
    in_port_t servPort;

    // Check if the number of arguments is sufficient
    if (argc < 5)
        DieWithUserMessage("Parameters", "<IP Version> <Server Port> -i <maze file>");

    int ipParameter = identifyIPVersion(argv[1]); // Check if IPv4, IPv6, or invalid
    if (ipParameter == 0)
        DieWithUserMessage("Parameter", "<IP Version> invalid!");

    int domain = (ipParameter == 1) ? AF_INET : AF_INET6;

    // Create socket for incoming connections
    int servSock; // Socket descriptor for server
    if ((servSock = socket(domain, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithSystemMessage("socket() failed");

    servPort = atoi(argv[2]); // Second arg: local port

    if (domain == AF_INET)
    {
        // Construct local address structure for IPv4
        struct sockaddr_in servAddr;
        memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
        servAddr.sin_family = domain;                 // IPv4 address family
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
        servAddr.sin_port = htons(servPort);          // Local port

        // Bind to the local address
        if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
            DieWithSystemMessage("bind() failed");
    }
    else if (domain == AF_INET6)
    {
        // Construct local address structure for IPv6
        struct sockaddr_in6 servAddr6;
        memset(&servAddr6, 0, sizeof(servAddr6)); // Zero out structure
        servAddr6.sin6_family = domain;           // IPv6 address family
        servAddr6.sin6_addr = in6addr_any;        // Any incoming interface
        servAddr6.sin6_port = htons(servPort);    // Local port

        // Bind to the local address
        if (bind(servSock, (struct sockaddr *)&servAddr6, sizeof(servAddr6)) < 0)
            DieWithSystemMessage("bind() failed");
    }

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, 1) < 0)
        DieWithSystemMessage("listen() failed");

    for (;;)
    {
        struct sockaddr_in clntAddr; // Client address
        // Set length of client address structure (in-out parameter)
        socklen_t clntAddrLen = sizeof(clntAddr);

        // Wait for a client to connect
        int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
        if (clntSock < 0)
            DieWithSystemMessage("accept() failed");

        // clntSock is connected to a client!

        char clntName[INET_ADDRSTRLEN]; // String to contain client address
        if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
            printf("client connected\n");
        else
            puts("Unable to get client address");

        handleGame(clntSock, argv[4]);
        close(clntSock);
        printf("client disconnected\n");
    }

    return 0;
}