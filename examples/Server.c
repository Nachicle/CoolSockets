#include <stdio.h>
#include <string.h>

#include <CoolSockets.h>

void toUpperCase(char* string) {
    for(int i=0; i<strlen(string); i++) {
        if(string[i] >= 'a' && string[i] <= 'z') {
            string[i] = string[i] - 'a' + 'A';
        }
    }
}
int main(int argc, char const *argv[]) {
    CoolSocket server;
    if(CS_ServerStart(&server, "0.0.0.0", 13337, CS_FAMILY_IPv4, CS_PROTOCOL_TCP)) {
        if(CS_ServerListen(server, 10)) {
            printf("Server started and listening! =D\nWaiting for client connection...\n");
            CoolSocket client;
            CS_ServerAccept(server, &client);
            printf("Client connected from %s[%d]!\n", client.address, client.port);
            while(1) {
                printf("Waiting for message...\n");
                char buffer[1024];
                int bytes = CS_Receive(client, buffer, sizeof(buffer));
                printf("Message received! Making it a bit louder and bringing it back...\n");
                toUpperCase(buffer);
                CS_Send(client, buffer, strlen(buffer)+1);
            }
        }
    } else {
        printf("Couldn't start server! )=\n");
    }
    return 0;
}