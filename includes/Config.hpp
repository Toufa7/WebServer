#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include <string>
#include "Shared.hpp"

class ServerLocation
{
    public:
        struct allowed_methods
        {
            bool post;
            bool get;
            bool del;
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
        void    parse_server_location(std::string location);
        void    print_server_location(unsigned int index);
};

class GlobalConfig
{
    public:
        unsigned int server_count;
        std::vector<ServerConfig> servers;
        void    parse_config_file(char *av);
        void    parse_server_config(std::string server);
        void    print_server_config(unsigned int index);
        void    print_servers(void);
        GlobalConfig();
};

void    InvalidConfigFile(std::string err_message);

#endif