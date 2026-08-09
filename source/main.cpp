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
// #include "dbglogger.h"

#define SCE_SYSTEM_SERVICE_EVENT_ON_RESUME  0x10000000u
#define SCE_SYSTEM_SERVICE_EVENT_BEFORE_SLEEP 0x10000001u

typedef struct SceSystemServiceEvent {
    uint32_t eventType;
    uint8_t  data[60];
} SceSystemServiceEvent;

extern "C"
{
    int sceNetInit(void);
    int sceNetPoolCreate(const char *name, int size, int flags);
    int sceNetPoolDestroy(int memid);
    int sceNetTerm(void);
    int sceKernelSendNotificationRequest(int, void *, size_t, int);
    int sceSystemServiceReceiveEvent(SceSystemServiceEvent *event);
}

static int g_libnet_mem_id = -1;
static bool in_rest_mode = false;
static bool stop_monitoring = false;

static void *SystemEventThread(void *argp)
{
    SceSystemServiceEvent event;

    while (!stop_monitoring)
    {
        int ret = sceSystemServiceReceiveEvent(&event);
        if (ret == 0)
        {
            switch (event.eventType)
            {
            case SCE_SYSTEM_SERVICE_EVENT_BEFORE_SLEEP:
                if (!in_rest_mode)
                {
                    in_rest_mode = true;
                    HttpServer::PauseDownloadThread();
                    Util::Notify("ezRemote: Pausing downloads for rest mode");
                }
                break;

            case SCE_SYSTEM_SERVICE_EVENT_ON_RESUME:
                if (in_rest_mode)
                {
                    in_rest_mode = false;
                    HttpServer::ResumeDownloadThread();
                    Util::Notify("ezRemote: Resuming downloads");
                }
                break;
            }
        }

        // Poll every 2 seconds
        sleep(2);
    }

    return nullptr;
}

static int NetInit(void)
{
    if (sceNetInit() != 0)
    {
        errno = EIO;
        return -1;
    }

    g_libnet_mem_id = sceNetPoolCreate("ezremote_server", 5 * 1024 * 1024, 0);
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

static void OnSignal()
{
    HttpServer::Stop();
    HttpServer::StopDownloadThread();
}

int main(int argc, char *argv[])
{
    if (NetInit() != 0)
    {
        NetTerm();
        return -1;
    }

    // dbglogger_init();
    // dbglogger_log("If you see this you've set up dbglogger correctly.");

    CONFIG::LoadPackageInstallHostData();
    CONFIG::LoadBgDownloadData();

    if (HttpServer::IsStarted())
    {
        Util::Notify("ezRemote Server already started");
        NetTerm();
        return 0;
    }

    atexit(OnSignal);

    // Start system event monitoring thread
    pthread_t sys_event_thread;
    pthread_create(&sys_event_thread, NULL, SystemEventThread, NULL);

    HttpServer::StartDownloadThread();
    HttpServer::Start();
    Util::Notify("ezRemote Server stopped.");
    HttpServer::StopDownloadThread();
    stop_monitoring = true;

    NetTerm();
    return 0;
}
