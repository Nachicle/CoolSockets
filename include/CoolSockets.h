#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define COOL_SOCKETS_VERSION "0.0.1"

/**
 * @brief 
 * 
 */
typedef enum {
    CS_FAMILY_IPv4,
    CS_FAMILY_IPv6
} CSFamily;

/**
 * @brief 
 * 
 */
typedef enum {
    CS_PROTOCOL_TCP,
    CS_PROTOCOL_UDP
} CSProtocol;

/**
 * @brief 
 * 
 */
typedef enum {
    CS_TYPE_CLIENT,
    CS_TYPE_SERVER
} CSType; 

/**
 * @brief 
 * 
 */
typedef enum {
    CS_RETURN_OK,
    CS_RETURN_ERROR,
    CS_RETURN_TIMEOUT
} CSReturnCode;

/**
 * @brief 
 * 
 */
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

/**
 * @brief 
 * 
 */
typedef CSReturnCode (*CSCallback)(CoolSocket, void*);

////////////////////// LIBRARY VERSION FUNCTIONS //////////////////////
/**
 * @brief Greets and prints library version.
 */
void CS_Version(void);

////////////////////// SERVER SPECIFIC FUNCTIONS //////////////////////
/**
 * @brief Starts a server socket.
 * @param server Pointer to the socket that will act as server.
 * @param address Address where clients will connect.
 * @param port Port where clients will connect.
 * @param family description
 * @param protocol description
 */
CSReturnCode CS_ServerStart(CoolSocket* server, char* address, int port, CSFamily family, CSProtocol protocol);
/**
 * @brief 
 * 
 */
CSReturnCode CS_ServerListen(CoolSocket server, int queueSize);
/**
 * @brief 
 * 
 */
CSReturnCode CS_ServerAccept(CoolSocket server, CoolSocket* client);
/**
 * @brief 
 * 
 */
CSReturnCode CS_ServerDisconnectClient(CoolSocket client);
/**
 * @brief 
 * 
 */
CSReturnCode CS_ServerStop(CoolSocket server);

////////////////////// CLIENT SPECIFIC FUNCTIONS //////////////////////
/**
 * @brief 
 * 
 */
CSReturnCode CS_ClientConnect(CoolSocket* client, char* address, int port, CSFamily family, CSProtocol protocol);

////////////////////// DATA TRANSFER FUNCTIONS //////////////////////
/**
 * @brief 
 * 
 */
int CS_Send(CoolSocket socket, void* buffer, int nbytes);
/**
 * @brief 
 * 
 */
CSReturnCode CS_SendAll(CoolSocket socket, void* buffer, int nbytes);
/**
 * @brief 
 * 
 */
int CS_Receive(CoolSocket socket, void* buffer, int nbytes);
/**
 * @brief 
 * 
 */
CSReturnCode cs_ReadAll(CoolSocket socket, void* buffer, int nbytes);

////////////////////// CALLBACK MANAGING FUNCTIONS //////////////////////
/**
 * @brief 
 * 
 */
void CS_SetConnectionCallback(CoolSocket* socket, CSCallback callback, void* callbackData);
/**
 * @brief 
 * 
 */
void CS_SetDisconnectionCallback(CoolSocket* socket, CSCallback callback, void* callbackData);
/**
 * @brief 
 * 
 */
void CS_SetDataReadyCallback(CoolSocket* socket, CSCallback callback, void* callbackData);
/**
 * @brief 
 * 
 */
void CS_ProcessSocketEvents(CoolSocket* socket);
/**
 * @brief 
 * 
 */
void CS_ProcessSocketArrayEvents(CoolSocket* socketArray, int arraySize);

#ifdef __cplusplus
}
#endif