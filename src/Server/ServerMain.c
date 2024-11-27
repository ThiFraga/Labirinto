#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Server.h"

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