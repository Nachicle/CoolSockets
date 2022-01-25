#include <stdio.h>

#include <CoolSockets.h>

int main(int argc, char const *argv[])
{
    CoolSocket client;
    CS_ClientConnect(&client, "127.0.0.1", 13337, CS_FAMILY_IPv4, CS_PROTOCOL_TCP);
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
