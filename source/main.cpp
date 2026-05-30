#undef main

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "server/http_server.h"
#include "config.h"
#include "util.h"
#include "dbglogger.h"

extern "C"
{
    int sceNetInit(void);
    int sceNetPoolCreate(const char *name, int size, int flags);
    int sceNetPoolDestroy(int memid);
    int sceNetTerm(void);
    int sceKernelSendNotificationRequest(int, void *, size_t, int);
}

static int g_libnet_mem_id = -1;
static volatile sig_atomic_t g_running = 1;

static int NetInit(void)
{
    if (sceNetInit() != 0)
    {
        errno = EIO;
        return -1;
    }

    g_libnet_mem_id = sceNetPoolCreate("nanodns", 5 * 1024 * 1024, 0);
    if (g_libnet_mem_id < 0)
    {
        errno = EIO;
        if (sceNetTerm() != 0)
        {
            errno = EIO;
        }
        return -1;
    }

    return 0;
}

static void NetTerm(void)
{
    if (g_libnet_mem_id >= 0)
    {
        if (sceNetPoolDestroy(g_libnet_mem_id) != 0)
        {
            errno = EIO;
        }
        g_libnet_mem_id = -1;
    }

    if (sceNetTerm() != 0)
    {
        errno = EIO;
    }
}

static void OnSignal(int signo)
{
    (void)signo;
    g_running = 0;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, OnSignal);
    signal(SIGTERM, OnSignal);

    if (NetInit() != 0)
    {
        NetTerm();
        return -1;
    }

    dbglogger_init();
    dbglogger_log("If you see this you've set up dbglogger correctly.");

    CONFIG::LoadPackageInstallHostData();
    CONFIG::LoadBgDownloadData();

    if (HttpServer::IsStarted())
    {
        Util::Notify("ezRemote Server already started");
        NetTerm();
        return 0;
    }

    HttpServer::StartDownloadThread();
    HttpServer::Start();
    Util::Notify("ezRemote Server stopped.");

    NetTerm();
    return 0;
}
