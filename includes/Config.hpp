#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include <string>
#include "Shared.hpp"

struct cgi
{
    std::string type;
    std::string path;
};

class ServerLocation
{
    /*
    TODO: add a redirection flag (bool var) to handle the case of a location that performs a redirection:
        location /old {
            return 301 http://example.com/new;
        }
        location /new {
            return 301 http://example.com/new;
        }
    */
    protected:
        std::string                 _LocationPath;
        int                         _AutoIndex;
        cgi                         _CgiInfo;
        std::string                 _Root;
        std::string                 _Upload;
        std::vector<std::string>    _AllowedMethodsVec;
    public:
        ServerLocation();
        ~ServerLocation();
        int                         GetAutoIndex(void);
        cgi&                        GetCgiInfo(void);
        std::string                 GetLocationPath(void);
        std::string                 GetRoot(void);
        std::string                 GetUpload(void);
        std::vector<std::string>    GetAllowedMethodsVec(void);
};

class ServerConfig : public ServerLocation
{
    protected:
        int                         _clientSocket;
        unsigned int                _Port;
        std::string                 _Host;
        std::string                 _ServerName;
        std::string                 _ClientBodySize;
        // TODO: note that we may have error page for each error code
        // soo changing this to somthing like map[key(error code)] = value(path to the page)
        // and validaitng that path brfore save it will be very usefull
        std::string                 _ErrorPage; 
        std::vector<ServerLocation> _LocationsVec;
    public:
        ServerConfig();
        ~ServerConfig();
        void                            ParseServerLocation(std::string location);
        void                            PrintServerLocation(unsigned int index);
        unsigned int                    GetPort(void);
        int                             getClientSocket(void);
        std::string                     GetHost(void);
        std::string                     GetServerName(void);
        std::string                     GetClientBodySize(void);
        std::string                     GetErrorPage(void);
        std::vector<ServerLocation>&    GetLocationsVec(void);

        void                            setClientSocket(int n);
};

class GlobalConfig : public ServerConfig
{
    protected:
        unsigned int _ServerCount;
        std::vector<ServerConfig> _ServersConfigVec;
    public:
        GlobalConfig();
        void    ParseConfigFile(char *av);
        void    ParseServerConfig(std::string server);
        void    PrintServerConfig(unsigned int index);
        void    PrintServers(void);
        unsigned int GetServerCount(void);
        std::vector<ServerConfig>& GetServersVector(void);

};

void    InvalidConfigFile(std::string err_message);

#endif