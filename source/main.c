#include "ps4.h"

#define LOADER_PORT 9021
#define MAX_PAYLOAD_SIZE 0x00800000

static void notify(const char *text)
{
    sceSysUtilSendSystemNotificationWithText(222, (char *)text);
}

int _main(void)
{
    initKernel();
    initLibc();
    initNetwork();
    initSysUtil();

    notify("payloadbyKG : loader TCP 9021");

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_len = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = sceNetHtons(LOADER_PORT);
    server_addr.sin_addr.s_addr = 0;

    int server = sceNetSocket("payloadbyKG", AF_INET, SOCK_STREAM, 0);
    if (server < 0)
    {
        notify("payloadbyKG : erreur socket");
        return -1;
    }

    if (sceNetBind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        notify("payloadbyKG : erreur bind 9021");
        sceNetSocketClose(server);
        return -2;
    }

    if (sceNetListen(server, 1) < 0)
    {
        notify("payloadbyKG : erreur listen");
        sceNetSocketClose(server);
        return -3;
    }

    notify("payloadbyKG : attente BIN sur 9021");

    int client = sceNetAccept(server, NULL, NULL);
    if (client < 0)
    {
        notify("payloadbyKG : erreur accept");
        sceNetSocketClose(server);
        return -4;
    }

    notify("payloadbyKG : PC connecte");

    /*
     * Ce loader reçoit un BIN plat et tente de l'exécuter directement.
     * L'environnement d'exploit doit autoriser une zone mémoire exécutable.
     */
    void *payload = mmap(
        NULL,
        MAX_PAYLOAD_SIZE,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    if (payload == MAP_FAILED)
    {
        notify("payloadbyKG : mmap RWX refuse");
        sceNetSocketClose(client);
        sceNetSocketClose(server);
        return -5;
    }

    unsigned char *dst = (unsigned char *)payload;
    unsigned long total = 0;

    while (total < MAX_PAYLOAD_SIZE)
    {
        unsigned int remaining = (unsigned int)(MAX_PAYLOAD_SIZE - total);
        unsigned int request = remaining > 0x10000 ? 0x10000 : remaining;

        int received = sceNetRecv(client, dst + total, request, 0);

        if (received <= 0)
            break;

        total += (unsigned long)received;
    }

    sceNetSocketClose(client);
    sceNetSocketClose(server);

    if (total == 0)
    {
        notify("payloadbyKG : aucun octet recu");
        return -6;
    }

    if (total >= MAX_PAYLOAD_SIZE)
    {
        notify("payloadbyKG : BIN trop grand");
        return -7;
    }

    notify("payloadbyKG : BIN recu, execution");

    /*
     * x86-64 : le cache instruction est cohérent.
     * Le BIN doit être un payload plat/position-independent compatible
     * avec l'environnement dans lequel ce loader a été lancé.
     */
    void (*entry)(void) = (void (*)(void))payload;
    entry();

    notify("payloadbyKG : payload termine");
    return 0;
}
