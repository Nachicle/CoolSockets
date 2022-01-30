#include <stdio.h>
#include <string.h>

#include <CoolSockets.h>

static CoolSocket _server = {0};
static CoolSocket _client = {0};

void toUpperCase(char* string) {
    for(int i=0; i<strlen(string); i++) {
        if(string[i] >= 'a' && string[i] <= 'z') {
            string[i] = string[i] - 'a' + 'A';
        }
    }
}

CSReturnCode clientDataReady(CoolSocket client, void* callbackData) {
    char buffer[1024] = {0};
    int bytes = CS_Receive(client, buffer, sizeof(buffer));
    printf("Client sent [%s] from %s\n", buffer, client.address);
    toUpperCase(buffer);
    CS_Send(client, buffer, strlen(buffer)+1);
    return CS_RETURN_OK;
}

CSReturnCode clientDisconnected(CoolSocket client, void* callbackData) {
    printf("Client disconnected from %s\n", client.address);
    return CS_RETURN_OK;
}

CSReturnCode clientConnected(CoolSocket client, void* callbackData) {
    _client = client;
    printf("Client connected from %s\n", client.address);
    CS_SetDataReadyCallback(&_client, clientDataReady, NULL);
    CS_SetDisconnectionCallback(&_client, clientDisconnected, NULL);
    return CS_RETURN_OK;
}

int main(int argc, char const *argv[]) {
    if(CS_ServerStart(&_server, "0.0.0.0", 13337, CS_FAMILY_IPv4, CS_PROTOCOL_TCP) == CS_RETURN_OK) {
        if(CS_ServerListen(_server, 10) == CS_RETURN_OK) {
            CS_SetConnectionCallback(&_server, clientConnected, NULL);
            while(1) {
                CS_ProcessSocketEvents(&_server);
                CS_ProcessSocketEvents(&_client);
            }    
        }
    } else {
        printf("Couldn't start server! )=\n");
    }
    return 0;
}