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
void cs_Version(void);

// Server specific functions
CSReturnCode cs_ServerStart(CoolSocket* server, char* address, int port, CSFamily family, CSType type);
CSReturnCode cs_ServerListen(CoolSocket server, int queueSize);
CSReturnCode cs_ServerAccept(CoolSocket server, CoolSocket* client) ;
//CSReturnCode cs_CloseServer(CoolSocket server);

// Data transfer functions
void cs_Write(void);
void cs_WriteAll(void);
int cs_Read(CoolSocket client, char* buffer, int bufferSize);
CSReturnCode cs_ReadAll(CoolSocket client, char* buffer, int toRead);