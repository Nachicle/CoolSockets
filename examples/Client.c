#include <stdio.h>

#include <CoolSockets.h>

int main(int argc, char const *argv[])
{
    CoolSocket client;
    CS_ClientConnect(&client, "fe80::a17f:274c:4579:967f", 13337, CS_FAMILY_IPv6, CS_TYPE_TCP);
    int counter = 0;
    char buffer[1024];
    while(1) {
        sprintf(buffer, "i'm SenDiNG messAgE nuMBEr %d", counter);
        CS_Send(client, buffer, sizeof(buffer));
        CS_Receive(client, buffer, sizeof(buffer));
        printf("%s\n", buffer);
        counter++;
    }
    return 0;
}
