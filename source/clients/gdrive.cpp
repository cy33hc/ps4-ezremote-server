#include <json-c/json.h>

#include "clients/gdrive.h"
#include "json_object_ref.h"

using namespace httplib;

#define GOOGLE_OAUTH_HOST "https://oauth2.googleapis.com"
#define GOOGLE_API_URL "https://www.googleapis.com"

GDriveClient::GDriveClient(){};

int GDriveClient::Connect(const std::string &url, const std::string &client_id, const std::string &secret, const std::string &refresh_token)
{
    Client tmp_client(GOOGLE_OAUTH_HOST);
    tmp_client.enable_server_certificate_verification(false);
    tmp_client.set_follow_location(true);
    std::string get_token_url = std::string("/token");
    std::string post_data = std::string("grant_type=refresh_token") +
                            "&client_id=" + client_id +
                            "&client_secret=" + secret +
                            "&refresh_token=" + refresh_token;

    if (auto res = tmp_client.Post(get_token_url, post_data.c_str(), post_data.length(), "application/x-www-form-urlencoded"))
    {
        if (HTTP_SUCCESS(res->status))
        {
            json_object *jobj = json_tokener_parse(res->body.c_str());
            JsonObjectRef jobj_ref(jobj);

            enum json_type type;
            json_object_object_foreach(jobj, key, val)
            {
                if (strcmp(key, "access_token") == 0)
                    this->access_token = std::string(json_object_get_string(val));
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

    return BaseClient::Connect(url, this->access_token);
}

uint32_t GDriveClient::GetMaxIdleTimeout() const
{
	return 55 * 60;
}