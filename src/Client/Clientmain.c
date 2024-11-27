#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Client.h"

int main(int argc, char *argv[])
{
    char input[50];
    int started = 0;
    action messageToSend, messageRecv;
    ssize_t numBytesRecv;

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
        else if (messageToSend.type == 1 && isValidMovement(messageToSend.moves[0], messageRecv.moves) == 0)
        {
            printf("error: you cannot go this way\n");
        }
        else
        {
            if (messageToSend.type == 0)
                started = 1;

            ssize_t numBytes = send(sock, &messageToSend, sizeof(messageToSend), 0);

            numBytesRecv = recv(sock, &messageRecv, sizeof(action), 0);
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
                        break;
                    case 2: // case asked for a map
                        printMap(messageRecv.board);
                        break;
                    case 3:
                        printHint(messageRecv.moves);
                    default:
                        break;
                    }

                    break;
                case 5: // handle win
                    printf("You escaped!\n");
                    printMap(messageRecv.board);
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