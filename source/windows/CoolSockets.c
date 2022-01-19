#include <CoolSockets.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

// Private logging functions
static void __cs_LogError(int errorCode) {
    char buffer[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT), buffer, sizeof(buffer), NULL);
    printf("[%d] %s\n", errorCode, buffer);
}

// Private address info functions
static int __cs_GetFamily(CSFamily family) {
    int result = -1;
    switch(family) {
        case CS_FAMILY_IPv4:
            result = AF_INET;
            break;
        case CS_FAMILY_IPv6:
            result = AF_INET6;
            break;
    }
    return result;
}
static int __cs_GetType(CSType type) {
    int result = -1;
    switch(type) {
        case CS_TYPE_TCP:
            result = SOCK_STREAM;
            break;
        case CS_TYPE_UDP:
            result = SOCK_DGRAM;
            break;
    }
    return result;
}
static int __cs_GetProtocol(CSType type) {
    int result = -1;
        switch(type) {
        case CS_TYPE_TCP:
            result = IPPROTO_TCP;
            break;
        case CS_TYPE_UDP:
            result = IPPROTO_UDP;
            break;
    }
    return result;
}

// Private Windows Socket API start function
static CSReturnCode __cs_WSAStartup(void) {
    static int wsaInit = FALSE;
    if(!wsaInit) {
        WSADATA wsaData = {0};
        int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if(wsaResult != 0) {
            __cs_LogError(wsaResult);
            return CS_RETURN_ERROR;
        } 
        wsaInit = TRUE;
    }
    return CS_RETURN_OK;
}

// Library version function
void CS_Version(void) {
    printf("Hi, I'm CoolSockets (%s) for Windows!\n", COOL_SOCKETS_VERSION);
}

// Server specific functions
CSReturnCode CS_ServerStart(CoolSocket* server, char* address, int port, CSFamily family, CSType type) {
    
    // Filling server data
    strcpy_s(server->address, sizeof(server->address), address);
    server->port = port;
    server->family = family;
    server->type = type;

    // Starting Windows Sockets API
    if(!__cs_WSAStartup()) {
        return CS_RETURN_ERROR;
    }
    
    // Getting address
    struct addrinfo *result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = __cs_GetFamily(server->family);
    hints.ai_socktype = __cs_GetType(server->type);
    hints.ai_protocol = __cs_GetProtocol(server->type);
    hints.ai_flags = AI_PASSIVE;

    char portString[16];
    sprintf_s(portString, sizeof(portString), "%d", server->port);

    int addrInfoResult = getaddrinfo(server->address, portString, &hints, &result);
    if (addrInfoResult != 0) {
        __cs_LogError(addrInfoResult);
        WSACleanup();
        return CS_RETURN_ERROR;
    }

    // Creating socket
    server->socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server->socket == INVALID_SOCKET) {
        __cs_LogError(WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return CS_RETURN_ERROR;
    }

    // Binding socket
    int bindingResult = bind(server->socket, result->ai_addr, (int)result->ai_addrlen);
    if (bindingResult == SOCKET_ERROR) {
        __cs_LogError(WSAGetLastError());
        closesocket(server->socket);
        freeaddrinfo(result);
        WSACleanup();
        return CS_RETURN_ERROR;
    }
    freeaddrinfo(result);
    
    return CS_RETURN_OK;
}
CSReturnCode CS_ServerListen(CoolSocket server, int queueSize) {
    if(listen(server.socket, queueSize) == SOCKET_ERROR) {
        __cs_LogError(WSAGetLastError());
        closesocket(server.socket);
        WSACleanup();
        return CS_RETURN_ERROR;
    }
    return CS_RETURN_OK;
}
CSReturnCode CS_ServerAccept(CoolSocket server, CoolSocket* client) {
    client->family = server.family;
    client->type = server.type;

    int clientAddrLen;
    switch(client->family) {
        case CS_FAMILY_IPv4:
            clientAddrLen = sizeof(struct sockaddr_in);
            break;
        case CS_FAMILY_IPv6:
            clientAddrLen = sizeof(struct sockaddr_in6);
            break;
    }

    void* clientAddr = malloc(clientAddrLen);
    client->socket = accept(server.socket, clientAddr, &clientAddrLen);

    if (client->socket == INVALID_SOCKET) {
        __cs_LogError(WSAGetLastError());
        closesocket(server.socket);
        WSACleanup();
        return CS_RETURN_ERROR;
    }

    switch(client->family) {
        case CS_FAMILY_IPv4:
            struct sockaddr_in* clientAddr4 = (struct sockaddr_in*) clientAddr;
            client->port = clientAddr4->sin_port;
            InetNtop(AF_INET, &clientAddr4->sin_addr, client->address, sizeof(client->address));
            break;
        case CS_FAMILY_IPv6:
            struct sockaddr_in6* clientAddr6 = (struct sockaddr_in6*) clientAddr;
            client->port = clientAddr6->sin6_port;
            InetNtop(AF_INET6, &clientAddr6->sin6_addr, client->address, sizeof(client->address));
            break;
    }

    return CS_RETURN_OK;
}
CSReturnCode CS_ServerDisconnectClient(CoolSocket client) {
    int disconnectionResult = shutdown(client.socket, SD_SEND);
    if(disconnectionResult == SOCKET_ERROR) {
        __cs_LogError(WSAGetLastError());
        return CS_RETURN_ERROR;
    }    
    int closeResult = closesocket(client.socket);
    if(!closeResult) {
        __cs_LogError(closeResult);
        return CS_RETURN_ERROR;
    }
    return CS_RETURN_OK;
}
CSReturnCode CS_ServerStop(CoolSocket server) {
    int closeResult = closesocket(server.socket);
    if(!closeResult) {
        __cs_LogError(closeResult);
        return CS_RETURN_ERROR;
    }
    return CS_RETURN_OK;
}

// Client specific functions
CSReturnCode CS_ClientConnect(CoolSocket* client, char* address, int port, CSFamily family, CSType type) {
    
    // Filling client data
    client->family = family;
    client->type = type;

    // Starting Windows Sockets API
    if(!__cs_WSAStartup()) {
        return CS_RETURN_ERROR;
    }

    // Getting address
    struct addrinfo *result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = __cs_GetFamily(client->family);
    hints.ai_socktype = __cs_GetType(client->type);
    hints.ai_protocol = __cs_GetProtocol(client->type);

    char portString[16];
    sprintf_s(portString, sizeof(portString), "%d", port);

    int addrInfoResult = getaddrinfo(address, portString, &hints, &result);
    if (addrInfoResult != 0) {
        __cs_LogError(addrInfoResult);
        WSACleanup();
        return CS_RETURN_ERROR;
    }

    // Creating socket
    client->socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (client->socket == INVALID_SOCKET) {
        __cs_LogError(WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return CS_RETURN_ERROR;
    } 

    // Connecting to server
    int connectionResult = connect(client->socket, result->ai_addr, result->ai_addrlen);
    if(connectionResult == SOCKET_ERROR) {
        closesocket(client->socket);

    }

    return CS_RETURN_OK;
}

// Data transfer functions
int CS_Send(CoolSocket socket, char* buffer, int nbytes) {
    return send(socket.socket, buffer, nbytes, 0);
}
CSReturnCode CS_SendAll(CoolSocket socket, char* buffer, int nbytes) {
    int written = 0;
    do {
        written += CS_Send(socket, buffer+written, nbytes-written);
    } while(written<nbytes);
    return CS_RETURN_OK;
}
int CS_Receive(CoolSocket socket, char* buffer, int nbytes) {
    return recv(socket.socket, buffer, nbytes, 0);
}
CSReturnCode cs_ReadAll(CoolSocket socket, char* buffer, int nbytes) {
    int read = 0;
    do {
        read += CS_Receive(socket, buffer+read, nbytes-read);
    } while(read<nbytes);
    return CS_RETURN_OK;
}