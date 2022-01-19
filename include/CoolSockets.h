#pragma once

#define COOL_SOCKETS_VERSION "0.0.1"

typedef enum {
    CS_FAMILY_IPv4,
    CS_FAMILY_IPv6
} CSFamily;

typedef enum {
    CS_TYPE_TCP,
    CS_TYPE_UDP
} CSType;

typedef enum {
    CS_RETURN_OK = 1,
    CS_RETURN_ERROR = 0
} CSReturnCode;

typedef struct {
    long long socket;
    char address[128];
    unsigned port;
    CSFamily family;
    CSType type;
} CoolSocket;

// Library version function
void CS_Version(void);

// Server specific functions
CSReturnCode CS_ServerStart(CoolSocket* server, char* address, int port, CSFamily family, CSType type);
CSReturnCode CS_ServerListen(CoolSocket server, int queueSize);
CSReturnCode CS_ServerAccept(CoolSocket server, CoolSocket* client);
CSReturnCode CS_ServerDisconnectClient(CoolSocket client);
CSReturnCode CS_ServerStop(CoolSocket server);

// Client specific functions
CSReturnCode CS_ClientConnect(CoolSocket* client, char* address, int port, CSFamily family, CSType type);

// Data transfer functions
int CS_Send(CoolSocket socket, char* buffer, int nbytes);
CSReturnCode CS_SendAll(CoolSocket socket, char* buffer, int nbytes);
int CS_Receive(CoolSocket socket, char* buffer, int nbytes);
CSReturnCode cs_ReadAll(CoolSocket socket, char* buffer, int nbytes);