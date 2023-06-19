#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <sys/stat.h>
#include "Shared.hpp"

struct cgi
{
    std::string type;
    std::string path;
};

struct error_page
{
    std::string code;
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
        cgi                         _CgiInfoPhp;
        cgi                         _CgiInfoPerl;
        redirection                 _RedirectionInfo;
        std::string                 _LocationPath;
        std::string                 _Root;
        std::string                 _Upload;
        std::vector<std::string>    _AllowedMethodsVec;
        std::vector<std::string>    _IndexesVec;
    public:
        ServerLocation();
        ~ServerLocation();
        ServerLocation(const ServerLocation & ServerObj);
        ServerLocation & operator = (const ServerLocation & ServerObj);
        int                         GetAutoIndex(void);
        cgi&                        GetCgiInfoPhp(void);
        cgi&                        GetCgiInfoPerl(void);
        redirection&                GetRedirectionInfo(void);
        std::string                 GetLocationPath(void);
        std::string                 GetRoot(void);
        std::string                 GetUpload(void);
        std::vector<std::string>    GetAllowedMethodsVec(void);
        std::vector<std::string>    GetIndexesVec(void);
};

class ServerConfig : public ServerLocation
{
    public:
        // int                                 ;
        int                                 _ServerSocket;
        unsigned int                        _Port;
        std::string                         _Host;
        std::string                         _ServerNames;
        std::string                         _ClientBodySize;
        std::vector<ServerLocation>         _LocationsVec;
        std::map<std::string, std::string>  _ErrorPageMap;
        ServerConfig();
        ~ServerConfig();
        ServerConfig(const ServerConfig & ServerObj);
        ServerConfig & operator = (const ServerConfig & ServerObj);
        int                                 GetServerSocket(void);
        unsigned int                        GetPort(void);
        std::string                         GetHost(void);
        std::string                         GetClientBodySize(void);
        std::string                         GetServerNames(void);
        void                                SetServerSocket(int socket);
        void                                ParseServerLocation(std::string location);
        void                                PrintServerLocation(unsigned int index);
        void                                setClientSocket(int n);
        std::vector<ServerLocation>&        GetLocationsVec(void);
        std::map<std::string, std::string>& GetErrorPageMap(void);
        void            ParseErrorPage(std::string error_directive);
};

class GlobalConfig
{
    protected:
        unsigned int                                        _ServerCount;
        std::vector<ServerConfig>                           _ServersConfigVec;
    public:
        GlobalConfig();
        GlobalConfig(const GlobalConfig & ServerObj);
        GlobalConfig & operator = (const GlobalConfig & ServerObj);
        ~GlobalConfig();
        void            ParseConfigFile(char *av);
        void            ParseServerConfig(std::string server);
        void            PrintServerConfig(unsigned int index);
        void            PrintServers(void);
        unsigned int    GetServerCount(void);
        std::vector<ServerConfig>& GetServersVector(void);
};

void    InvalidConfigFile(std::string err_message);

#endif