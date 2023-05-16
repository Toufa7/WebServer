#ifndef CONFIG_H
#define CONFIG_H

#include "Shared.hpp"
#include "Handler.hpp"
#include "Server.hpp"
#include "Client.hpp"

class ServerLocation
{
    public:
        struct allowed_methods
        {
            int post;
            int get;
            int del;
        };

        struct cgi
        {
            std::string type;
            std::string path;
        };

        std::string     location_path;
        int             auto_index;
        cgi             cgi_info;
        std::string     root;
        std::string     upload;
        allowed_methods alwd_mtd;
        ServerLocation();
        ~ServerLocation();  
};

class ServerConfig
{
    public:
        unsigned int                _port;
        std::string                 _host;
        std::string                 _server_name;
        std::string                 _client_body_size;
        std::string                 _error_page;
        std::vector<ServerLocation> _locations;

    ServerConfig();
    ~ServerConfig();
};

class GlobalConfig
{
    public:
        std::vector<ServerConfig> servers;
        static unsigned int server_index;
    void    parse_config_file(char *av); 
    void    parse_server_config(std::string server);
    void    parse_server_location(std::string location);
};

#endif