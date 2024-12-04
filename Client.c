#include "Common.h"

/* Summary
    Client Functions line 8
    Main Functions line 206
*/

// Client Functions

void cleanInput(char *str)
{
    char *end;

    // remove spaces from beginning
    while (isspace((unsigned char)*str))
        str++;

    // remove spaces from ending
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';
}

Action getMessage(char *msg)
{
    Action newMessage;
    for (int i = 0; i < 100; i++)
    {
        newMessage.moves[i] = 0; // initialize moves vector
    }

    cleanInput(msg); // clean user command

    // compare user command with possible commands
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
    else if (strcmp(msg, "hint") == 0)
    {
        newMessage.type = 3;
    }
    else
    {
        newMessage.type = 10;
    }

    return newMessage;
}

void getMoveName(char *move, int value)
{
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

void printPossibleMoves(int moves[])
{
    int i = 0;
    char moveName[MAXMOVENAMESIZE];

    fputs("Possible moves: ", stdout);

    while (moves[i] != 0 && i < MAXMOVES)
    {
        getMoveName(moveName, moves[i]);
        if (i != 0)
            fputs(", ", stdout);
        fputs(moveName, stdout);
        i++;
    }
    fputs(".\n", stdout);
}

void printHint(int moves[])
{
    int i = 0;
    char moveName[MAXMOVENAMESIZE];

    fputs("Hint: ", stdout);

    while (moves[i] != 0 && i < MAXMOVES)
    {
        getMoveName(moveName, moves[i]);
        if (i != 0)
            fputs(", ", stdout);
        fputs(moveName, stdout);
        i++;
    }
    fputs(".\n", stdout);
}

char *getCharEquivalent(int value)
{
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
        res = "@";
        break;
    }
    return res;
}

void printMap(int maze[MAXMAZESIZE][MAXMAZESIZE])
{
    char *c, *first;
    for (int i = 0; i < MAXMAZESIZE; i++)
    {
        first = getCharEquivalent(maze[i][0]);
        for (int j = 0; j < MAXMAZESIZE; j++)
        {
            c = getCharEquivalent(maze[i][j]);
            if (strcmp(c, "@") != 0)
                printf("%s\t", c);
        }
        if (strcmp(first, "@") != 0)
            printf("\n");
    }
}

int isValidMovement(int movement, int possibleMoves[MAXMOVES])
{
    int res = 0;
    for (int i = 0; i < MAXMOVES; i++)
    {
        if (possibleMoves[i] == movement)
        {
            res = 1;
            break;
        }
    }
    return res;
}

// Main Function
int main(int argc, char *argv[])
{
    char input[50];
    int possibleMoves[MAXMOVES], started = 0, needToReset = 0;
    Action messageToSend, messageRecv;
    ssize_t numBytesRecv;

    for (int i = 0; i < MAXMOVES; i++)
        possibleMoves[i] = 0;

    // Check number of arguments
    if (argc != 3)
        DieWithUserMessage("Parameter(s)", "<Server Address> <Server Port>");

    char *servIP = argv[1];             // First arg: server IP address (string format)
    in_port_t servPort = atoi(argv[2]); // Second arg: server port (numeric)

    int domain;                       // Domain type (AF_INET or AF_INET6)
    struct sockaddr_storage servAddr; // Supports both IPv4 and IPv6
    socklen_t servAddrLen;

    // Detect IP type (IPv4 or IPv6)
    if (inet_pton(AF_INET, servIP, &((struct sockaddr_in *)&servAddr)->sin_addr) == 1)
    {
        // IPv4 address
        domain = AF_INET;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)&servAddr;
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(servPort);
        servAddrLen = sizeof(struct sockaddr_in);
    }
    else if (inet_pton(AF_INET6, servIP, &((struct sockaddr_in6 *)&servAddr)->sin6_addr) == 1)
    {
        // IPv6 address
        domain = AF_INET6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&servAddr;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(servPort);
        servAddrLen = sizeof(struct sockaddr_in6);
    }
    else
    {
        DieWithUserMessage("inet_pton() failed", "Invalid IP address format");
    }

    // Create a reliable, stream socket using TCP
    int sock = socket(domain, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
        DieWithSystemMessage("socket() failed");

    // Establish the connection to the server
    if (connect(sock, (struct sockaddr *)&servAddr, servAddrLen) < 0)
        DieWithSystemMessage("connect() failed");

    // game loop
    for (;;)
    {
        fgets(input, sizeof(input), stdin); // command input

        messageToSend = getMessage(input);

        // error treatment
        if (messageToSend.type == 10)
        {
            printf("error: command not found\n");
        }
        else if (started == 0 && messageToSend.type != 0)
        {
            printf("error: start the game first\n");
        }
        else if(needToReset == 1 && messageToSend.type != 6 && messageToSend.type != 7) 
        {
            printf("error: reset the game first\n");
        }
        else if (messageToSend.type == 1 && isValidMovement(messageToSend.moves[0], possibleMoves) == 0)
        {
            printf("error: you cannot go this way\n");
        }
        else
        {
            if (messageToSend.type == 0)
                started = 1;
            else if (messageToSend.type == 6)
                needToReset = 0;

            send(sock, &messageToSend, sizeof(messageToSend), 0);

            numBytesRecv = recv(sock, &messageRecv, sizeof(Action), 0);
            if (numBytesRecv > 0)
            {
                switch (messageRecv.type)
                {
                case 4: // handle update
                    switch (messageToSend.type)
                    {
                    case 0: // case started, moved or reseted
                    case 1:
                    case 6:
                        printPossibleMoves(messageRecv.moves);
                        for (int i = 0; i < MAXMOVES; i++)
                            possibleMoves[i] = messageRecv.moves[i];
                        break;
                    case 2: // case asked for a map
                        printMap(messageRecv.board);
                        break;
                    case 3: // case asked for a hint
                        printHint(messageRecv.moves);
                    default:
                        break;
                    }

                    break;
                case 5: // handle win
                    printf("You escaped!\n");
                    printMap(messageRecv.board);
                    needToReset = 1;
                    break;

                default:
                    break;
                }
            }
        }
        if (messageToSend.type == 7)
            break;
    }

    close(sock);
    return 0;
}