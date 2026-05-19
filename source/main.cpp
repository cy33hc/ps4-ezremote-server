#undef main

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <orbis/SystemService.h>
#include <orbis/Net.h>

#include "server/http_server.h"
#include "config.h"
#include "util.h"
#include "dbglogger.h"

extern "C"
{
#include "orbis_jbc.h"
}

static void terminate()
{
    terminate_jbc();
    sceSystemServiceLoadExec("exit", NULL);
}

int main(int argc, char *argv[])
{
    dbglogger_init();
    dbglogger_log("If you see this you've set up dbglogger correctly.");

    if (!initialize_jbc())
    {
        terminate();
    }

    atexit(terminate);

    if (sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE) < 0) return 0;
    if (sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE) < 0) return 0;
    if (sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NET) < 0 || sceNetInit() != 0) return 0;

    CONFIG::LoadPackageInstallHostData();
    CONFIG::LoadBgDownloadData();

    if (HttpServer::IsStarted())
    {
        Util::Notify("ezRemote Server already started");
        terminate();
        return 0;
    }

    dbglogger_log(" Registering Daemon...");
    sceSystemServiceRegisterDaemon();

    HttpServer::StartDownloadThread();
    HttpServer::Start();
    Util::Notify("ezRemote Server stopped.");

    return 0;
}
