#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define COOL_SOCKETS_VERSION "0.0.1"

typedef enum {
    CS_FAMILY_IPv4,
    CS_FAMILY_IPv6
} CSFamily;

typedef enum {
    CS_PROTOCOL_TCP,
    CS_PROTOCOL_UDP
} CSProtocol;

typedef enum {
    CS_TYPE_CLIENT,
    CS_TYPE_SERVER
} CSType; 

typedef enum {
    CS_RETURN_OK,
    CS_RETURN_ERROR,
    CS_RETURN_TIMEOUT
} CSReturnCode;

typedef struct CoolSocket {
    // Socket info
    long long socket;
    char address[128];
    unsigned port;
    CSFamily family;
    CSProtocol protocol;
    CSType type;
    int blocking;
    // Callback fields
    CSReturnCode (*connectionCallback)(struct CoolSocket, void*);
    void* connectionCallbackData;
    CSReturnCode (*disconnectionCallback)(struct CoolSocket, void*);
    void* disconnectionCallbackData;
    CSReturnCode (*dataReadyCallback)(struct CoolSocket, void*);
    void* dataReadyCallbackData;
} CoolSocket;

typedef CSReturnCode (*CSCallback)(CoolSocket, void*);

// Library version function
void CS_Version(void);

// Server specific functions
CSReturnCode CS_ServerStart(CoolSocket* server, char* address, int port, CSFamily family, CSProtocol protocol);
CSReturnCode CS_ServerListen(CoolSocket server, int queueSize);
CSReturnCode CS_ServerAccept(CoolSocket server, CoolSocket* client);
CSReturnCode CS_ServerDisconnectClient(CoolSocket client);
CSReturnCode CS_ServerStop(CoolSocket server);

// Client specific functions
CSReturnCode CS_ClientConnect(CoolSocket* client, char* address, int port, CSFamily family, CSProtocol protocol);

// Data transfer functions
int CS_Send(CoolSocket socket, void* buffer, int nbytes);
CSReturnCode CS_SendAll(CoolSocket socket, void* buffer, int nbytes);
int CS_Receive(CoolSocket socket, void* buffer, int nbytes);
CSReturnCode cs_ReadAll(CoolSocket socket, void* buffer, int nbytes);

// Callback managing functions
void CS_SetConnectionCallback(CoolSocket* socket, CSCallback callback, void* callbackData);
void CS_SetDisconnectionCallback(CoolSocket* socket, CSCallback callback, void* callbackData);
void CS_SetDataReadyCallback(CoolSocket* socket, CSCallback callback, void* callbackData);
void CS_ProcessSocketEvents(CoolSocket* socket);
void CS_ProcessSocketArrayEvents(CoolSocket* socketArray, int arraySize);

#ifdef __cplusplus
}
#endif