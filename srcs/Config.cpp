#include "../includes/Config.hpp"

//ServerLocation OCF
ServerLocation::ServerLocation()
{
    _RedirectionInfo.RedirectionFlag = false;
    _RedirectionInfo.RedirectionCode = "n/a";
    _RedirectionInfo.RedirectionPath = "n/a";

    _CgiInfoPhp.type = "n/a";
    _CgiInfoPhp.path = "n/a";

    _CgiInfoPerl.type = "n/a";
    _CgiInfoPerl.path = "n/a";

    _LocationPath = "n/a";
    _AutoIndex = 0;
    _Root = "n/a";
    _Upload = 0;   
}

bool ServerLocation::operator < (const ServerLocation & ServerObj)
{
    return this->GetLocationPath() < ServerObj._LocationPath;
}

bool ServerLocation::operator > (const ServerLocation & ServerObj)
{
    return this->GetLocationPath() > ServerObj._LocationPath;
}


ServerLocation::ServerLocation(const ServerLocation & ServerObj)
{
    *this = ServerObj;
}

ServerLocation & ServerLocation::operator = (const ServerLocation & ServerObj)
{
    this->_AutoIndex = ServerObj._AutoIndex;
    this->_CgiInfoPhp.path = ServerObj._CgiInfoPhp.path;
    this->_CgiInfoPhp.type = ServerObj._CgiInfoPhp.type;
    this->_CgiInfoPerl.path = ServerObj._CgiInfoPerl.path;
    this->_CgiInfoPerl.type = ServerObj._CgiInfoPerl.type;
    this->_RedirectionInfo.RedirectionFlag = ServerObj._RedirectionInfo.RedirectionFlag;
    this->_RedirectionInfo.RedirectionCode = ServerObj._RedirectionInfo.RedirectionCode;
    this->_RedirectionInfo.RedirectionPath = ServerObj._RedirectionInfo.RedirectionPath;
    this->_LocationPath = ServerObj._LocationPath;
    this->_Root = ServerObj._Root;
    this->_Upload = ServerObj._Upload;
    this->_AllowedMethodsVec = ServerObj._AllowedMethodsVec;
    this->_IndexesVec = ServerObj._IndexesVec;
    return *this;
}

ServerLocation::~ServerLocation()
{
    
}
//ServerLocation OCF end

//ServerConfig OCF
ServerConfig::ServerConfig()
{
    _Port = 8080;
    _Host = "n/a";
    _ClientBodySize = "n/a";
}

ServerConfig::ServerConfig(const ServerConfig & ServerObj)
{
    *this = ServerObj;
}

ServerConfig & ServerConfig::operator = (const ServerConfig & ServerObj)
{
    // this->_clientSocket = ServerObj._clientSocket;
    this->_ServerSocket = ServerObj._ServerSocket;
    this->_Port = ServerObj._Port;
    this->_Host = ServerObj._Host;
    this->_ServerNames = ServerObj._ServerNames;
    this->_ClientBodySize = ServerObj._ClientBodySize;
    this->_LocationsVec = ServerObj._LocationsVec;
    this->_ErrorPageMap = ServerObj._ErrorPageMap;
    return *this;
}

ServerConfig::~ServerConfig()
{

}
//ServerConfig OCF end

//GlobalConfig OCF
GlobalConfig::GlobalConfig()
{
    _ServerCount = 0;
}

GlobalConfig::GlobalConfig(const GlobalConfig & ServerObj)
{
    *this = ServerObj;
}

GlobalConfig & GlobalConfig::operator = (const GlobalConfig & ServerObj)
{
    this->_ServerCount = ServerObj._ServerCount;
    this->_ServersConfigVec = ServerObj._ServersConfigVec;
    return *this;
}

GlobalConfig::~GlobalConfig()
{
    
}
//GlobalConfig OCF end

void    InvalidConfigFile(std::string err_message)
{
    std::cout << err_message << std::endl;
    exit(1);
}

void GlobalConfig::ParseConfigFile(char *av)
{
    int             dot_position = 0, oc = 0, soc = 0;//oc: occurrence of some things, soc: "server" substring occurrence in string
    std::string     buffer, read_data, file_name(av), server;
    std::ifstream   config_file;
    
    /*----------------------------------- Reading config file ---------------------------------------*/
    dot_position = file_name.find_last_of('.');
    if (dot_position > 0)
    {
        if (file_name.substr(dot_position ,file_name.length()) != ".conf")
            InvalidConfigFile("Invalid config file: wrong extension.");
    }
    else
        InvalidConfigFile("Invalid config file: wrong extension.");
    config_file.open(file_name);
    if (!config_file)
        InvalidConfigFile("Invalid config file: there was an error when opening config file.");
    while (config_file)
    {
        std::getline(config_file, buffer);
        read_data += buffer += '\n';
    }
    //For the last '\n'
    read_data.pop_back();
    /*------------------------------------- End of reading -----------------------------------------*/


    /*----------------------------------- Extracting each "server" ----------------------------------*/
    oc = read_data.find("server ");
    if (oc >= 0)
    {
        while (oc >= 0)
        {
            soc = read_data.find("server ", oc + 1);
            if (soc > oc)//since oc is already bigger than 0,so for soc to be after oc, it should be be bigger
            {
                server = read_data.substr(oc, soc - oc);
                oc = read_data.find("server ", soc + 1);
                ParseServerConfig(server);
            }
            if (soc < oc && soc > 0)
            {
                server = read_data.substr(soc, oc - soc);
                soc = read_data.find("server ", oc + 1);
                ParseServerConfig(server);
            }
            if ( soc < 0 || oc < 0 || oc == 0 ) // if soc < 0 that means there's only one server, (soc < 0 || oc < 0) in case error happened break from the loop
            {
                if (soc > 0)
                    server = read_data.substr(soc, read_data.length());
                else if (oc > 0)
                    server = read_data.substr(oc, read_data.length());
                else if (oc == 0)
                    server = read_data.substr(oc, read_data.length());
                ParseServerConfig(server);//call to this f() to parse each server individually
                break;
            }
        }
    }
    else
        InvalidConfigFile("Invalid config file : server directive not found");
    /*--------------------------------- End of extracting "server" ----------------------------------*/
}

void    GlobalConfig::ParseServerConfig(std::string server)
{
    int key_pos, colon_pos, value_pos, scolon_pos;
    std::string location, tmp_str;
    ServerConfig tmp;

    /*------------------------------- Error check for server string end -----------------------------------*/
    for (unsigned int i = server.length() - 1; i > 0; i--)
    {
        if (server[i] == '}' || server[i] == '\n')
        {
            if (server[i] == '}')
            {
                if (server[i - 1] == '\n')
                {
                    if (server[i - 2] == '}')
                        break;
                    else
                        InvalidConfigFile("Invalid config file : invalid server closing, closing brace should be proceeded by last location closing brace");
                }
                else
                    InvalidConfigFile("Invalid config file : invalid server closing, closing brace should be proceeded by last location closing brace");
            }
            else
                continue;
        }
        else
            InvalidConfigFile("Invalid config file : invalid server closing");
    }
    /*---------------------------------------- Error check end --------------------------------------------*/


    _ServerCount++;
    /*------------------------------- host and port -----------------------------------*/
    key_pos = server.find("listen ");
    if (key_pos >= 0)
    {
        colon_pos = server.find(":", key_pos);
        if (colon_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find Host Port).");
        tmp._Host = server.substr(key_pos + 7, (colon_pos - (key_pos + 7)));
        value_pos = server.find(";", colon_pos);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error.");
        tmp._Port = std::atoi(server.substr(colon_pos + 1, (((value_pos - colon_pos) - 1))).c_str());
        if (tmp._Port < 0 || tmp._Port > 65536)
            InvalidConfigFile("Invalid config file : Wrong port number");
    }
    else
        InvalidConfigFile("Invalid config file : listen directive is not found.");
    /*------------------------------- End of host port --------------------------------*/

    /*---------------------------- server_names -----------------------------------*/
    key_pos = server.find("server_names ");
    if (key_pos > 0)
    {
        scolon_pos = server.find(";", (key_pos + 13));
        if (scolon_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find Server Names).");
        tmp._ServerNames = server.substr((key_pos + 13), scolon_pos - (key_pos + 13));
        value_pos = tmp._ServerNames.find(" ");
        if (value_pos != -1)
            InvalidConfigFile("Invalid config file : Wrong error page directive.");

        //std::cout << "--->" << tmp._ServerNames << "\n";
        /* In case we wanted to split each server names
        std::stringstream ss(tmp._ServerNames);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> vstrings(begin, end);
        std::cout << "--->" << vstrings[1] << "\n";
         */
    }
    else
        InvalidConfigFile("Invalid config file : server names directive is not found.");
    /*---------------------------- End of server_names --------------------------------*/

    /*--------------------------------- client body size -------------------------------------*/
    key_pos = server.find("client_body_size ");
    if (key_pos >= 0)
    {
        scolon_pos = server.find(";", (key_pos + 17));
        if (scolon_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find Client Body Size).");
        tmp._ClientBodySize = server.substr((key_pos + 17), scolon_pos - (key_pos + 17));
    }
    else
        InvalidConfigFile("Invalid config file : client body size is not found or not valid.");
    /*---------------------------- End of client body size -----------------------------------*/

    /*-------------------------------------- error page ---------------------------------------*/
    key_pos = server.find("error_page ");
    if (key_pos > 0)
    {
        while (1)
        {
            if (key_pos >= 0)
            {
                scolon_pos = server.find(";", (key_pos + 11));
                if (scolon_pos < 0)
                    InvalidConfigFile("Invalid config file : There was an error (Find Error Page).");
                tmp_str = server.substr((key_pos + 11), scolon_pos - (key_pos + 11));
                tmp.ParseErrorPage(tmp_str);
            }
            else
                break;
            key_pos = server.find("error_page ", key_pos + 1);

        }
    }
    /*----------------------------------- end of error page -----------------------------------*/

    /*------------------------------------- find locations -------------------------------------*/

    key_pos = server.find("location ");
    if (key_pos > 0)
    {
        while (1)
        {
            if (key_pos >= 0)
            {
                value_pos = server.find("}", key_pos);
                if (value_pos < 0)
                    InvalidConfigFile("Invalid config file : There was an error (Find Loactions).");
                location = server.substr(key_pos, (value_pos - key_pos) + 1);
                tmp.ParseServerLocation(location);
            }
            else
                break;
            key_pos = server.find("location ", key_pos + 1);
        }
    }
    else
        InvalidConfigFile("Invalid config file : location directive not found.");
    /*----------------------------------- End find locations -----------------------------------*/

    _ServersConfigVec.push_back(tmp);
}

void    ServerConfig::ParseServerLocation(std::string location)
{
    int key_pos, value_pos, npos = 0;
    ServerConfig location_tmp;
    std::string tmp_str;

    /*------------------------------------- find location path -------------------------------------*/
    key_pos = location.find("location ");
    if (key_pos < 0)
        InvalidConfigFile("Invalid config file : There was an error (Find Location Path).");
    value_pos = location.find("{", key_pos + 1);
    if (value_pos < 0)
        InvalidConfigFile("Invalid config file : There was an error (Find Location Path).");
    location_tmp._LocationPath = location.substr((key_pos + 9), value_pos - (key_pos + 10));
    /*----------------------------------- end find location path -----------------------------------*/

    /*----------------------------------------- allowed methods ----------------------------------------*/
    key_pos = location.find("allow_methods ");
    if (key_pos > 0)
    {
        value_pos = location.find(";", key_pos + 1);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find allowedmwthodes).");
        tmp_str = location.substr((key_pos + 14), value_pos - (key_pos + 14));

        npos = tmp_str.find("GET");
        if (npos >= 0)
            location_tmp._AllowedMethodsVec.push_back("GET");
        npos = tmp_str.find("POST");
        if (npos >= 0)
            location_tmp._AllowedMethodsVec.push_back("POST");
        npos = tmp_str.find("DELETE");
        if (npos >= 0)
            location_tmp._AllowedMethodsVec.push_back("DELETE");
        tmp_str.erase();
    }
    else
        InvalidConfigFile("Invalid config file : allowed methods not found (Find allowed methodes).");
    /*------------------------------------- End of allowed methods -------------------------------------*/

    /*------------------------------------------ autoindex ----------------------------------------*/
    key_pos = location.find("autoindex ");
    if (key_pos > 0)
    {
        value_pos = location.find(";", key_pos + 1);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find autoindex).");
        tmp_str = location.substr((key_pos + 10), value_pos - (key_pos + 10));
        if (tmp_str.find("on") != std::string::npos)
            location_tmp._AutoIndex = 1;
        tmp_str.erase();
    }
    else
        InvalidConfigFile("Invalid config file : auto index not found (Find autoindex).");
    /*------------------------------------- End of auto index -------------------------------------*/

        /*------------------------------------------ upload ----------------------------------------*/
    key_pos = location.find("upload ");
    if (key_pos > 0)
    {
        value_pos = location.find(";", key_pos + 1);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find upload).");
        tmp_str = location.substr((key_pos + 7), value_pos - (key_pos + 7));
        if (tmp_str.find("on") != std::string::npos)
            location_tmp._Upload = 1;
        tmp_str.erase();
    }
    else
        InvalidConfigFile("Invalid config file : auto index not found (Find upload).");
    /*------------------------------------- upload index -------------------------------------*/

    /*------------------------------------- find root -------------------------------------*/
    key_pos = location.find("root ");
    if (key_pos >= 0)
    {
        value_pos = location.find(";", key_pos + 1);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find Root).");
        location_tmp._Root = location.substr((key_pos + 5), value_pos - (key_pos + 5));
    }
    else
        InvalidConfigFile("Invalid config file : root directive is not found.");

    /*----------------------------------- end find root -----------------------------------*/
    
   /*------------------------------------- find cgi -----------------------------------------*/
   //PHP CGI
    struct stat interpreterPhp;

    key_pos = location.find("cgi ");
    if (key_pos >= 0)
    {
        value_pos = location.find(";", key_pos + 1);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find CGI).");
        tmp_str = location.substr((key_pos + 4), value_pos - (key_pos + 4));
        npos = tmp_str.find(" ", 1);
        if (npos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find CGI).");
        location_tmp._CgiInfoPhp.type = tmp_str.substr(0, npos);
        npos = tmp_str.find(" ");
        if (npos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find CGI).");
        location_tmp._CgiInfoPhp.path = tmp_str.substr(tmp_str.find(" ") + 1, value_pos - (tmp_str.find(" ") + 1));
        
        if (stat(location_tmp._CgiInfoPhp.path.c_str(), &interpreterPhp) != 0)
            InvalidConfigFile("Invalid config file : Invalid interpreter path (Find PHP CGI).");
        
        tmp_str.erase();
    }

    //Perl CGI
    struct stat interpreterPerl;
    key_pos = location.find("cgi ", key_pos + 1);
    if (key_pos >= 0)
    {
        value_pos = location.find(";", key_pos + 1);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find CGI).");
        tmp_str = location.substr((key_pos + 4), value_pos - (key_pos + 4));
        npos = tmp_str.find(" ", 1);
        if (npos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find CGI).");
        location_tmp._CgiInfoPerl.type = tmp_str.substr(0, npos);
        npos = tmp_str.find(" ");
        if (npos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find CGI).");
        location_tmp._CgiInfoPerl.path = tmp_str.substr(tmp_str.find(" ") + 1, value_pos - (tmp_str.find(" ") + 1));
        
        if (stat(location_tmp._CgiInfoPerl.path.c_str(), &interpreterPerl) != 0)
            InvalidConfigFile("Invalid config file : Invalid interpreter path (Find PERL CGI).");
        
        tmp_str.erase();
    }
    /*----------------------------------- end of find cgi -----------------------------------*/

    /*------------------------------------- find redirection -----------------------------------------*/
    key_pos = location.find("return ");
    if (key_pos >= 0)
    {
        location_tmp._RedirectionInfo.RedirectionFlag = true;
        value_pos = location.find(";", key_pos + 1);
        if (value_pos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find Redirection).");
        tmp_str = location.substr((key_pos + 7), value_pos - (key_pos + 7));
        npos = tmp_str.find(" ", 1);
        if (npos < 0)
            InvalidConfigFile("Invalid config file : There was an error (Find Redirection).");
        location_tmp._RedirectionInfo.RedirectionCode = tmp_str.substr(0, npos);
        location_tmp._RedirectionInfo.RedirectionPath = tmp_str.substr(npos + 1, value_pos - (npos + 1));
        tmp_str.erase();
    }
    /*----------------------------------- end of find redirection -----------------------------------*/

    /*----------------------------------------- find indexes ----------------------------------------*/
    key_pos = location.find("indexes ");
    if (key_pos > 0)
    {
        struct stat indexFileStat;
        std::string indexFilePath;

        while (1)
        {
            if (key_pos >= 0)
            {
                value_pos = location.find("\n", key_pos + 1);
                if (value_pos < 0)
                    InvalidConfigFile("Invalid config file : There was an error (Find Index).");
                indexFilePath = location_tmp._Root += '/';
                indexFilePath += location.substr((key_pos + 8), value_pos - (key_pos + 9));
                if ((stat(indexFilePath.c_str(), &indexFileStat) == 0) && (indexFileStat.st_mode & S_IFREG))
                    location_tmp._IndexesVec.push_back(location.substr((key_pos + 8), value_pos - (key_pos + 9)));
            }
            else
                break;
            key_pos = location.find("indexes ", key_pos + 1);
        }
    }
    /*-------------------------------------- end of find indexes ------------------------------------*/
    this->_LocationsVec.push_back(location_tmp);
}

void    GlobalConfig::PrintServerConfig(unsigned int index)
{
    if (index <= (_ServerCount - 1)) {
        std::cout << "Server index              : " << index << "\n";
        std::cout << "Server host               : " << _ServersConfigVec[index].GetHost() << "\n";
        std::cout << "Server port               : " << _ServersConfigVec[index].GetPort() << "\n";
        if (_ServersConfigVec[index].GetServerNames() != "n/a")
            std::cout << "Server name               : " << _ServersConfigVec[index].GetServerNames() << "\n";
        std::cout << "Server client body size   : " << _ServersConfigVec[index].GetClientBodySize() << "\n";

         if (_ServersConfigVec[index].GetErrorPageMap().empty() == false)
         {
             std::map<std::string, std::string>::iterator it;

             std::cout << "\nServer error page(s)      : " << "\n";
             for (it = _ServersConfigVec[index].GetErrorPageMap().begin(); it != _ServersConfigVec[index].GetErrorPageMap().end(); ++it)
             {
                 std::cout << "Error code        : " << it->first << "\n";
                 std::cout << "Error page path   : " << it->second << "\n";
             }
         }
    
        std::cout << "\nServer location(s)\n\n";
        for (unsigned long i = 0; i < _ServersConfigVec[index].GetLocationsVec().size(); i++) {
            _ServersConfigVec[index].PrintServerLocation(i);
            std::cout << "\n";
        }
    }
}

 void ServerConfig::PrintServerLocation(unsigned int index)
 {
     if (index <= _LocationsVec.size())
     {
        std::cout << "Location index            : " << index << "\n";
        std::cout << "Location path             : " << _LocationsVec[index].GetLocationPath() << "\n";
        std::cout << "Location root             : " << _LocationsVec[index].GetRoot() << "\n";
        std::cout << "Location auto index       : " ;
        if (_LocationsVec[index].GetAutoIndex() == 0)
            std::cout << "OFF";
        else
            std::cout << "OFF";
        std::cout << "\n";

        if (_LocationsVec[index].GetCgiInfoPhp().type != "n/a")
        {
            std::cout << "Location cgi type         : " << _LocationsVec[index].GetCgiInfoPhp().type << "\n";
            std::cout << "Location cgi path         : " << _LocationsVec[index].GetCgiInfoPhp().path << "\n";
        }

        // if (_LocationsVec[index].GetUpload() != "n/a")
        //     std::cout << "server _Upload         : " << _LocationsVec[index].GetUpload() << "\n";
        
        if (_LocationsVec[index].GetAllowedMethodsVec().size() > 0)
        {
            std::cout << "Location allowed methods  : " ;
            for (unsigned int i = 0; i < _LocationsVec[index].GetAllowedMethodsVec().size(); i++)
                std::cout << _LocationsVec[index].GetAllowedMethodsVec()[i] << " ";
        }

        std::cout << "\n";
        
        if (_LocationsVec[index].GetRedirectionInfo().RedirectionFlag == true)
        {
            std::cout << "Redirection code          : " << _LocationsVec[index].GetRedirectionInfo().RedirectionCode << "\n";
            std::cout << "Redirection path          : " << _LocationsVec[index].GetRedirectionInfo().RedirectionPath << "\n";
        }
        std::cout << "\n";
     }
 }

void    GlobalConfig::PrintServers(void)
{
    for (unsigned int i = 0; i < _ServersConfigVec.size(); i++)
        PrintServerConfig(i);
}

void ServerConfig::ParseErrorPage(std::string error_directive)
{

    std::stringstream ss(error_directive);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> SplitedStringsVec(begin, end);

    if (SplitedStringsVec.empty() != 1)
        this->_ErrorPageMap[SplitedStringsVec[0]] = SplitedStringsVec[1];
    else
        InvalidConfigFile("Invalid config file : There was an error.");
}

// --------------------- ACCESSOR ----------------

std::vector<ServerConfig>& GlobalConfig::GetServersVector(void)
{
    return (_ServersConfigVec);
}

unsigned int GlobalConfig::GetServerCount(void)
{
    return (_ServerCount);
}

unsigned int ServerConfig::GetPort(void)
{
    return (_Port);
}

std::string ServerConfig::GetHost(void)
{
    return (_Host);
}

std::string ServerConfig::GetServerNames(void)
{
    return (_ServerNames);
}

std::string ServerConfig::GetClientBodySize(void)
{
    return (_ClientBodySize);
}

std::vector<ServerLocation>& ServerConfig::GetLocationsVec(void)
{
    return  (_LocationsVec);
}

int ServerLocation::GetAutoIndex(void)
{
    return (_AutoIndex);
}

int ServerLocation::GetUpload(void)
{
    return (_Upload);
}

cgi& ServerLocation::GetCgiInfoPhp(void)
{
    return (_CgiInfoPhp);
}

cgi& ServerLocation::GetCgiInfoPerl(void)
{
    return (_CgiInfoPerl);
}

std::string ServerLocation::GetLocationPath(void)
{
    return (_LocationPath);
}

std::string ServerLocation::GetRoot(void)
{
    return (_Root);
}

// std::string ServerLocation::GetUpload(void)
// {
//     return (_Upload);
// }

std::vector<std::string> ServerLocation::GetAllowedMethodsVec(void)
{
    return (_AllowedMethodsVec);
}

void    ServerConfig::SetServerSocket(int socket)
{
    //Do not forget to check for errors
    _ServerSocket = socket;
}

int ServerConfig::GetServerSocket(void)
{
    return (_ServerSocket);
}

redirection&    ServerLocation::GetRedirectionInfo(void)
{
    return (_RedirectionInfo);
}

std::map<std::string, std::string>& ServerConfig::GetErrorPageMap(void)
{
    return (_ErrorPageMap);
}

std::vector<std::string> ServerLocation::GetIndexesVec(void)
{
    return (_IndexesVec);
}