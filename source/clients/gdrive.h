#ifndef EZ_GDRIVE_H
#define EZ_GDRIVE_H

#include <string>
#include <vector>
#include "http/httplib.h"
#include "clients/remote_client.h"
#include "clients/baseclient.h"

class GDriveClient : public BaseClient
{
public:
    GDriveClient();
    int Connect(const std::string &url, const std::string &client_id, const std::string &secret, const std::string &refresh_token);
    uint32_t GetMaxIdleTimeout() const override;

private:
    std::string access_token;
};

#endif
