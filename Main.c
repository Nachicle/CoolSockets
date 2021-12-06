#include <stdio.h>

#include <CoolSockets.h>

int main(int argc, char const *argv[]) {

    cs_Version();
    CoolSocket server;    

    /*if(cs_ServerStart(&server, "fe80::25d2:b19d:78f6:1db6", 12334, CS_FAMILY_IPv6, CS_TYPE_TCP)) {
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
    }*/

    CoolSocket client;
    cs_ClientConnect(&client, "fe80::25d2:b19d:78f6:1db6", 1338, CS_FAMILY_IPv6, CS_TYPE_TCP);
    char buffer[1024];
    while (1) {
        gets_s(buffer, sizeof(buffer));
        cs_Write(client, buffer, strlen(buffer)+1);
        cs_Read(client, buffer, sizeof(buffer));
        printf("%s\n", buffer);
    }
    

    return 0;
}
