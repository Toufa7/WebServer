#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include "Shared.hpp"

struct cgi
{
    std::string type;
    std::string path;
};

struct redirection
{
    bool        RedirectionFlag;
    std::string RedirectionCode;
    std::string RedirectionPath;
};

class ServerLocation
{
    protected:
        int                         _AutoIndex;
        cgi                         _CgiInfo;
        redirection                 _RedirectionInfo;
        std::string                 _LocationPath;
        std::string                 _Root;
        std::string                 _Upload;
        std::vector<std::string>    _AllowedMethodsVec;
    public:
        ServerLocation();
        ~ServerLocation();
        int                         GetAutoIndex(void);
        cgi&                        GetCgiInfo(void);
        redirection&                GetRedirectionInfo(void);
        std::string                 GetLocationPath(void);
        std::string                 GetRoot(void);
        std::string                 GetUpload(void);
        std::vector<std::string>    GetAllowedMethodsVec(void);
};

class ServerConfig : public ServerLocation
{
    protected:
        unsigned int                _Port;
        int                         _ServerSocket;
        std::string                 _Host;
        std::string    _ServerNames;
        std::string                 _ClientBodySize;
        // TODO: note that we may have error page for each error code
        // soo changing this to somthing like map[key(error code)] = value(path to the page)
        // and validaitng that path brfore save it will be very usefull
        std::string                 _ErrorPage; 
        std::vector<ServerLocation> _LocationsVec;
    public:
        ServerConfig();
        ~ServerConfig();
        int                             GetServerSocket(void);
        unsigned int                    GetPort(void);
        std::string                     GetHost(void);
        std::string                     GetClientBodySize(void);
        std::string                     GetErrorPage(void);
        void                            SetServerSocket(int socket);
        void                            ParseServerLocation(std::string location);
        void                            PrintServerLocation(unsigned int index);
        std::string        GetServerNames(void);
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