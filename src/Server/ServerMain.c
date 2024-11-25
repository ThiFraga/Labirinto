#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Server.h"

int main(int argc, char *argv[])
{
    FILE *arq;
    in_port_t servPort;

    // check if the number of arguments is suficient
    if (argc < 5)
        DieWithUserMessage("Parameters", "<IP Version> <Server Port> -i <maze file>");

    int ipParameter = identifyIPVersion(argv[1]); // check if is IPv4 IPv6 or invalid
    if (ipParameter == 0)
        DieWithUserMessage("Parameter", "<IP Version> invalid!");

    int domain;
    if (ipParameter == 1)
        domain = AF_INET;
    else if (ipParameter == 2)
        domain = AF_INET6;

    // Create socket for incoming connections
    int servSock; // Socket descriptor for server
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithSystemMessage("socket() failed");

    servPort = atoi(argv[2]); // First arg: local port

    // Construct local address structure
    struct sockaddr_in servAddr;                  // Local address
    memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
    servAddr.sin_family = domain;                 // IPv4 or IPv6 address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
    servAddr.sin_port = htons(servPort);          // Local port

    // Bind to the local address
    if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("bind() failed");

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, 1) < 0)
        DieWithSystemMessage("listen() failed");

    for(;;) {
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
    }
    

    printf("fim");
    return 0;
}