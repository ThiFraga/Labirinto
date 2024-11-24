#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Client.h"

struct action
{
    int type;
    int moves[100];
    int board[10][10];
};

int main(int argc, char *argv[]) {
    char input[50];
    int started = 0;
    int numberInput = 10;
    int moveDirection = 0;
    
    // check number of arguments
    if(argc != 3) DieWithUserMessage("Parameter(s)", "<Server Address> <Server Port>");

    char *servIP = argv[1]; // First arg: server IP address (dotted quad)

    // Third arg (optional): server port (numeric). 7 is well-known echo port
    in_port_t servPort = atoi(argv[2]);

    // Create a reliable, stream socket using TCP
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) DieWithSystemMessage("socket() failed");

    // Construct the server address structure
    struct sockaddr_in servAddr; // Server address
    memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
    servAddr.sin_family = AF_INET; // IPv4 address family
    // Convert address

    int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (rtnVal == 0) DieWithUserMessage("inet_pton() failed", "invalid address string");
    else if (rtnVal < 0) DieWithSystemMessage("inet_pton() failed");
    servAddr.sin_port = htons(servPort); // Server port

    // Establish the connection to the echo server
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) DieWithSystemMessage("connect() failed");

    // game loop
    for(;;){
        fgets(input,sizeof(input),stdin);
        


    }
    return 0;
}