#include <CoolSockets.h>

int main(int argc, char const *argv[])
{
    CoolSocket socket;
    CS_ServerStart(&socket, "0.0.0.0", 1337, CS_FAMILY_IPv4, CS_PROTOCOL_TCP);
    
    return 0;
}
