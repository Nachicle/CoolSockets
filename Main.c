#include <CoolSockets.h>

int main(int argc, char const *argv[]) {
    
    cs_Version();
    CoolSocket server;    

    if(cs_ServerStart(&server, "fe80::25d2:b19d:78f6:1db6", 12334, CS_FAMILY_IPv6, CS_TYPE_TCP)) {
        printf("\nServer opened successfully!\n");
        printf("\nSocket: %lld\nAddress: %s\nPort: %d\nFamily: %d\nType: %d\n\n", server.socket, server.address, server.port, server.family, server.type);
        cs_ServerListen(server, 10);
        CoolSocket client;
        cs_ServerAccept(server, &client);
        printf("\nSocket: %lld\nAddress: %s\nPort: %d\nFamily: %d\nType: %d\n", client.socket, client.address, client.port, client.family, client.type);
    } else {
        printf("Couldn't open server D=\n");
    }

    return 0;
}
