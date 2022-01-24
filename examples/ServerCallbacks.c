#include <stdio.h>
#include <string.h>

#include <CoolSockets.h>

static CoolSocket server = {0};
static CoolSocket _client = {0};

CSReturnCode clientSendData(CoolSocket client) {
    char buffer[512] = {0};
    CS_Receive(client, buffer, sizeof(buffer));
    printf("Client sent [%s] from %s\n", buffer, client.address);
    return CS_RETURN_OK;
}

CSReturnCode clientDisconnected(CoolSocket client) {
    printf("Client disconnected from %s\n", client.address);
    return CS_RETURN_OK;
}

CSReturnCode clientConnected(CoolSocket client) {
    _client = client;
    printf("Client connected from %s\n", client.address);
    CS_SetDataReadyCallback(&_client, clientSendData);
    CS_SetDisconnectionCallback(&_client, clientDisconnected);
    return CS_RETURN_OK;
}

int main(int argc, char const *argv[]) {
    if(CS_ServerStart(&server, "0.0.0.0", 13337, CS_FAMILY_IPv4, CS_TYPE_TCP)) {
        if(CS_ServerListen(server, 10)) {
            CS_SetConnectionCallback(&server, clientConnected);
            while(1) {
                CS_ProcessSocketEvents(server);
                CS_ProcessSocketEvents(_client);
            }    
        }
    } else {
        printf("Couldn't start server! )=\n");
    }
    return 0;
}