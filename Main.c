#include <stdio.h>

#include <CoolSockets.h>

int main(int argc, char const *argv[]) {

    cs_Version();
    CoolSocket server;    

    if(cs_ServerStart(&server, "fe80::25d2:b19d:78f6:1db6", 12334, CS_FAMILY_IPv6, CS_TYPE_TCP)) {
        cs_ServerListen(server, 10);
        
        CoolSocket client;
        cs_ServerAccept(server, &client);

        char buffer[1024];
        while(1) {
            cs_Read(client, buffer, 1024);
            printf("%s", buffer);
            cs_Write(client, buffer, strlen(buffer));
        }

    } else {
        printf("Couldn't open server D=\n");
    }

    return 0;
}
