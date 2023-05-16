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
        unsigned int                _Port;
        std::string                 _Host;
        std::string                 _ServerName;
        std::string                 _ClientBodySize;
        std::string                 _ErrorPage;
        std::vector<ServerLocation> _LocationsVec;
    public:
        ServerConfig();
        ~ServerConfig();
        void                            ParseServerLocation(std::string location);
        void                            PrintServerLocation(unsigned int index);
        unsigned int                    GetPort(void);
        std::string                     GetHost(void);
        std::string                     GetServerName(void);
        std::string                     GetClientBodySize(void);
        std::string                     GetErrorPage(void);
        std::vector<ServerLocation>&    GetLocationsVec(void);

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