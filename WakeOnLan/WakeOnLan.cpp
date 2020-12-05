#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <vcruntime_exception.h>
#include "hnrt/SocketLibrary.h"
#include "hnrt/Socket.h"
#include "hnrt/InternetAddress.h"
#include "hnrt/MacAddress.h"
#include "hnrt/MagicPacket.h"
#include "hnrt/Exception.h"


#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Socket")
#pragma comment(lib, "Core")


using namespace hnrt;


int main(int argc, char* argv[])
{
    try
    {
        setlocale(LC_ALL, "");

        if (argc != 3)
        {
            fprintf(stderr, "ERROR: Bad command line syntax. Specify MAC address and IPv4 broadcast address.\n");
            goto failure;
        }

        MacAddress mac;
        if (!mac.Parse(argv[1]))
        {
            fprintf(stderr, "ERROR: Malformed MAC address string: %s\n", argv[1]);
            goto failure;
        }

        SocketLibrary sockLib;

        InternetAddress addr;

        addr.Port = 7;

        if (!addr.SetHost(argv[2]))
        {
            fprintf(stderr, "ERROR: Malformed IPv4 address string: %s\n", argv[2]);
            goto failure;
        }

        Socket sd;
        
        int rc = sd.Create(AF_INET, SOCK_DGRAM);
        if (rc)
        {
            fprintf(stderr, "ERROR: Socket is not available: %d\n", rc);
            goto failure;
        }

        MagicPacket packet(mac);

        int sent = sendto(sd, &packet, packet.Len, 0, &addr, addr.Len);
        if (sent == packet.Len)
        {
            printf("MAGIC packet for %s (%d bytes) has been sent successfully.\n", mac.ToString(), packet.Len);
        }
        else
        {
            fprintf(stderr, "ERROR: Failed to sent MAGIC packet: %d\n", WSAGetLastError());
            goto failure;
        }

        return EXIT_SUCCESS;
    }
    catch (std::bad_alloc)
    {
        fwprintf(stderr, L"ERROR: Out of memory.\n");
    }
    catch (Exception e)
    {
        fwprintf(stderr, L"ERROR: %s\n", e.Message);
    }
    catch (...)
    {
        fwprintf(stderr, L"ERROR: Unhandled exception was caught.\n");
    }

failure:

    return EXIT_FAILURE;
}
