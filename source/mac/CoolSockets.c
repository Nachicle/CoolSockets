 #include <CoolSockets.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>

// Library version function
void CS_Version(void) {
    printf("Hi, I'm CoolSockets (%s) for Mac!\n", COOL_SOCKETS_VERSION);
}

// Server specific functions
CSReturnCode CS_ServerStart(CoolSocket* server, char* address, int port, CSFamily family, CSProtocol protocol) {
    
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind(server->socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(server->socket, 10); 

    CoolSocket client;
    client.socket = accept(server->socket, (struct sockaddr*)NULL, NULL); 

    printf("client accepted");

    return CS_RETURN_OK;
}