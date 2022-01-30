#include <CoolSockets.h>

#include <stdio.h>
#include <stdlib.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

// Private logging functions
static void __CS_LogError(int wsaErrorCode) {
    char buffer[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, wsaErrorCode, MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT), buffer, sizeof(buffer), NULL);
    printf("[%d] %s\n", wsaErrorCode, buffer);
}
#define __CS_LogErrorExtended(wsaErrorCode) {\
    printf("\nFollowing error ocurred at %s[%d] during %s call:\n", __FILENAME__, __LINE__, __FUNCTION__);\
    __CS_LogError(wsaErrorCode);\
} 

// Private address info functions
static int __CS_GetFamily(CSFamily family) {
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
static int __CS_GetSockType(CSProtocol protocol) {
    int result = -1;
    switch(protocol) {
        case CS_PROTOCOL_TCP:
            result = SOCK_STREAM;
            break;
        case CS_PROTOCOL_UDP:
            result = SOCK_DGRAM;
            break;
    }
    return result;
}
static int __CS_GetProtocol(CSProtocol protocol) {
    int result = -1;
        switch(protocol) {
        case CS_PROTOCOL_TCP:
            result = IPPROTO_TCP;
            break;
        case CS_PROTOCOL_UDP:
            result = IPPROTO_UDP;
            break;
    }
    return result;
}

// Private Windows Socket API helpers
static CSReturnCode __cs_WSAStartup(void) {
    static int wsaInit = FALSE;
    if(!wsaInit) {
        WSADATA wsaData = {0};
        int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if(wsaResult != 0) {
            __CS_LogErrorExtended(wsaResult);
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
CSReturnCode CS_ServerStart(CoolSocket* server, char* address, int port, CSFamily family, CSProtocol protocol) {
    
    // Initializing socket data
    memset(server, 0, sizeof(CoolSocket));
    server->type = CS_TYPE_SERVER;
    server->blocking = TRUE;

    // Filling server data
    strcpy_s(server->address, sizeof(server->address), address);
    server->port = port;
    server->family = family;
    server->protocol = protocol;

    // Starting Windows Sockets API
    if(__cs_WSAStartup() == CS_RETURN_ERROR) {
        return CS_RETURN_ERROR;
    }
    // Getting address
    struct addrinfo *result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = __CS_GetFamily(server->family);
    hints.ai_socktype = __CS_GetSockType(server->protocol);
    hints.ai_protocol = __CS_GetProtocol(server->protocol);
    hints.ai_flags = AI_PASSIVE;

    char portString[16];
    sprintf_s(portString, sizeof(portString), "%d", server->port);

    int addrInfoResult = getaddrinfo(server->address, portString, &hints, &result);
    if (addrInfoResult != 0) {
        __CS_LogErrorExtended(addrInfoResult);
        WSACleanup();
        return CS_RETURN_ERROR;
    }

    // Creating socket
    server->socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server->socket == INVALID_SOCKET) {
        __CS_LogErrorExtended(WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return CS_RETURN_ERROR;
    }

    // Binding socket
    int bindingResult = bind(server->socket, result->ai_addr, (int)result->ai_addrlen);
    if (bindingResult == SOCKET_ERROR) {
        __CS_LogErrorExtended(WSAGetLastError());
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
        __CS_LogErrorExtended(WSAGetLastError());
        closesocket(server.socket);
        WSACleanup();
        return CS_RETURN_ERROR;
    }
    return CS_RETURN_OK;
}
CSReturnCode CS_ServerAccept(CoolSocket server, CoolSocket* client) {

    // Initializing socket data
    memset(client, 0, sizeof(CoolSocket));
    client->type = CS_TYPE_CLIENT;
    client->blocking = TRUE;

    client->family = server.family;
    client->protocol = server.protocol;

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
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            return CS_RETURN_TIMEOUT;
        }
        __CS_LogErrorExtended(WSAGetLastError());
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
        __CS_LogErrorExtended(WSAGetLastError());
        return CS_RETURN_ERROR;
    }    
    int closeResult = closesocket(client.socket);
    if(!closeResult) {
        __CS_LogErrorExtended(closeResult);
        return CS_RETURN_ERROR;
    }
    return CS_RETURN_OK;
}
CSReturnCode CS_ServerStop(CoolSocket server) {
    int closeResult = closesocket(server.socket);
    if(!closeResult) {
        __CS_LogErrorExtended(closeResult);
        return CS_RETURN_ERROR;
    }
    return CS_RETURN_OK;
}

// Client specific functions
CSReturnCode CS_ClientConnect(CoolSocket* client, char* address, int port, CSFamily family, CSProtocol protocol) {
    
    // Initializing socket data
    memset(client, 0, sizeof(CoolSocket));
    client->type = CS_TYPE_CLIENT;
    client->blocking = TRUE;

    // Filling client data
    client->family = family;
    client->protocol = protocol;

    // Starting Windows Sockets API
    if(__cs_WSAStartup() == CS_RETURN_ERROR) {
        return CS_RETURN_ERROR;
    }

    // Getting address
    struct addrinfo *result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = __CS_GetFamily(client->family);
    hints.ai_socktype = __CS_GetSockType(client->protocol);
    hints.ai_protocol = __CS_GetProtocol(client->protocol);

    char portString[16];
    sprintf_s(portString, sizeof(portString), "%d", port);

    int addrInfoResult = getaddrinfo(address, portString, &hints, &result);
    if (addrInfoResult != 0) {
        __CS_LogErrorExtended(addrInfoResult);
        WSACleanup();
        return CS_RETURN_ERROR;
    }

    // Creating socket
    client->socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (client->socket == INVALID_SOCKET) {
        __CS_LogErrorExtended(WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return CS_RETURN_ERROR;
    } 

    // Connecting to server
    int connectionResult = connect(client->socket, result->ai_addr, result->ai_addrlen);
    if(connectionResult == SOCKET_ERROR) {
        closesocket(client->socket);
        return CS_RETURN_ERROR;
    }

    return CS_RETURN_OK;
}

// Data transfer functions
int CS_Send(CoolSocket socket, void* buffer, int nbytes) {
    return send(socket.socket, (char*)buffer, nbytes, 0);
}
CSReturnCode CS_SendAll(CoolSocket socket, void* buffer, int nbytes) {
    int written = 0;
    do {
        written += CS_Send(socket, (char*)buffer+written, nbytes-written);
    } while(written<nbytes);
    return CS_RETURN_OK;
}
int CS_Receive(CoolSocket socket, void* buffer, int nbytes) {
    return recv(socket.socket, (char*)buffer, nbytes, 0);
}
CSReturnCode cs_ReadAll(CoolSocket socket, void* buffer, int nbytes) {
    int read = 0;
    do {
        read += CS_Receive(socket, (char*)buffer+read, nbytes-read);
    } while(read<nbytes);
    return CS_RETURN_OK;
}

// Callback managing functions
void CS_SetConnectionCallback(CoolSocket* socket, CSCallback callback, void* callbackData) {
    socket->connectionCallback = callback;
    socket->connectionCallbackData = callbackData;
}
void CS_SetDisconnectionCallback(CoolSocket* socket, CSCallback callback, void* callbackData) {
    socket->disconnectionCallback = callback;
    socket->disconnectionCallbackData = callbackData;
}
void CS_SetDataReadyCallback(CoolSocket* socket, CSCallback callback, void* callbackData) {
    socket->dataReadyCallback = callback;
    socket->dataReadyCallbackData = callbackData;
}
void CS_ProcessSocketEvents(CoolSocket* socket) {
    
    // Checking if socket is valid
    if(socket->socket < 0) {
        return;
    }
    
    // Unblocking socket
    if(socket->blocking) {
        int mode = 1;
        int result = ioctlsocket(socket->socket, FIONBIO, &mode);
        socket->blocking = FALSE;
    }

    // Checking for connections
    if(socket->connectionCallback) {
        CoolSocket client;
        if(CS_ServerAccept(*socket, &client) == CS_RETURN_OK) {
            socket->connectionCallback(client);
        }
    }
    
    // Checking for disconnections and data ready
    if(socket->dataReadyCallback || socket->disconnectionCallback) {
        char aux_buffer[1];
        if(recv(socket->socket, aux_buffer, 1, MSG_PEEK) == SOCKET_ERROR) {
            if(WSAGetLastError() != WSAEWOULDBLOCK) {
                socket->disconnectionCallback(socket);
                closesocket(socket->socket);
                socket->socket = -1;
            }
        } else {
            socket->dataReadyCallback(socket);
        }
    }
   
}
void CS_ProcessSocketArrayEvents(CoolSocket* socketArray, int arraySize) {
    if(socketArray) {
        for(int i = 0; i < arraySize; i++) {
            CS_ProcessSocketEvents(&socketArray[i]);
        }
    }
}