#include "../includes/Config.hpp"

void    InvalidConfigFile(std::string err_message)
{
    std::cout << err_message << std::endl;
    exit(1);
}

ServerLocation::ServerLocation()
{
    //all ints shall be intialised by 0
    //and all strings by n/a 'not applicable'

    _CgiInfo.type = "n/a";
    _CgiInfo.path = "n/a";

    _LocationPath = "n/a";
    _AutoIndex = FALSE;
    _Root = "n/a";
    _Upload = "n/a";   
}

ServerLocation::~ServerLocation()
{
    //ConfigLocation destructor
}

ServerConfig::ServerConfig()
{
    _Port = 8080;
    _Host = "n/a";
    _ServerName = "n/a";
    _ClientBodySize = "n/a";
    _ErrorPage = "n/a";   
}

ServerConfig::~ServerConfig()
{
    //config destructor
}

GlobalConfig::GlobalConfig()
{
    _ServerCount = 0;
}

void GlobalConfig::ParseConfigFile(char *av)
{
    //int           error_code;
    int             dot_position = 0, oc = 0, soc = 0; //oc: ocurence of somethings, soc: "server" substring occurence in string
    std::string     buffer, read_data, file_name(av), server;
    std::ifstream   config_file;
    
    /*----------------------------------- Reading config file ---------------------------------------*/
    dot_position = file_name.find_last_of('.');
    if (dot_position > 0)
    {
        if (file_name.substr(dot_position ,file_name.length()) != ".conf")
            InvalidConfigFile("Invalid config file: wrong extention.");

    }
    else
        InvalidConfigFile("Invalid config file: wrong extension.");
    config_file.open(file_name);
    if (!config_file)
        InvalidConfigFile("There was an error when opening config file.");
    while (config_file)
    {
        std::getline(config_file, buffer);
        read_data += buffer += '\n';
    }
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
            if (soc < 0 || oc < 0) // if soc < 0 that means there's only one server
            {
                if (soc > 0)
                    server = read_data.substr(soc, read_data.length());
                else if (oc > 0)
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
    std::string location;
    GlobalConfig tmp;

    _ServerCount++;
    /*------------------------------- host and port -----------------------------------*/
    key_pos = server.find("listen ");
    if (key_pos >= 0)
    {
        colon_pos = server.find(":", key_pos);
        tmp._Host = server.substr(key_pos + 7, (colon_pos - (key_pos + 7)));
        value_pos = server.find(";", colon_pos);
        tmp._Port = std::atoi(server.substr(colon_pos + 1, (((value_pos - colon_pos) - 1))).c_str());
    }
    else
        InvalidConfigFile("Invalid config file : listen directive is not found.");
    /*------------------------------- End of host port --------------------------------*/

    /*---------------------------- server_name -----------------------------------*/
    key_pos = server.find("server_name ");
    if (key_pos > 0)
    {
        scolon_pos = server.find(";", (key_pos + 12));
        tmp._ServerName = server.substr((key_pos + 12), scolon_pos - (key_pos + 12));
    }
    /*---------------------------- End of server_name --------------------------------*/

    /*--------------------------------- client body size -------------------------------------*/
    key_pos = server.find("client_body_size ");
    if (key_pos >= 0)
    {
        scolon_pos = server.find(";", (key_pos + 17));
        tmp._ClientBodySize = server.substr((key_pos + 17), scolon_pos - (key_pos + 17));
    }
    else
        InvalidConfigFile("Invalid config file : client body size is not found or not valid.");
    /*---------------------------- End of client body size -----------------------------------*/

    /*-------------------------------------- error page ---------------------------------------*/
    key_pos = server.find("error_page ");
    if (key_pos > 0)
    {
        scolon_pos = server.find(";", (key_pos + 11));
        tmp._ErrorPage = server.substr((key_pos + 11), scolon_pos - (key_pos + 11));
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
    int key_pos, value_pos, npos;
    ServerConfig location_tmp;
    std::string tmp_str;

    /*------------------------------------- find location path -------------------------------------*/
    key_pos = location.find("location ");
    value_pos = location.find("{", key_pos + 1);
    location_tmp._LocationPath = location.substr((key_pos + 9), value_pos - (key_pos + 9));
    /*----------------------------------- end find location path -----------------------------------*/

    /*----------------------------------------- allowed methods ----------------------------------------*/
    key_pos = location.find("allow_methods ");
    if (key_pos > 0)
    {
        value_pos = location.find(";", key_pos + 1);
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
        InvalidConfigFile("Invalid config file : allowed methods not found.");
    /*------------------------------------- End of allowed methods -------------------------------------*/

    /*----------------------------------------- auto index ----------------------------------------*/
    key_pos = location.find("autoindex ");
    if (key_pos > 0)
    {
        value_pos = location.find(";", key_pos + 1);
        tmp_str = location.substr((key_pos + 10), value_pos - (key_pos + 10));
        if (tmp_str.find("on") >= 0)
            location_tmp._AutoIndex = 1;
    }
    else
        InvalidConfigFile("Invalid config file : auto index not found.");
    /*------------------------------------- End of auto index -------------------------------------*/

    /*------------------------------------- find root -------------------------------------*/
    key_pos = location.find("root ");
    if (key_pos >= 0)
    {
        value_pos = location.find(";", key_pos + 1);
        location_tmp._Root = location.substr((key_pos + 5), value_pos - (key_pos + 5));
    }
    else
        InvalidConfigFile("Invalid config file : root directive is not found.");

    /*----------------------------------- end find root -----------------------------------*/
    
   /*------------------------------------- find cgi -----------------------------------------*/
    key_pos = location.find("cgi ");

    if (key_pos >= 0)
    {
        value_pos = location.find(";", key_pos + 1);
        tmp_str = location.substr((key_pos + 4), value_pos - (key_pos + 4));
        location_tmp._CgiInfo.type = tmp_str.substr(0, tmp_str.find(" ", 1));
        location_tmp._CgiInfo.path = tmp_str.substr(tmp_str.find(" ") + 1, value_pos - (tmp_str.find(" ") + 1));
    }
    /*----------------------------------- end of find cgi -----------------------------------*/
    this->_LocationsVec.push_back(location_tmp);
}

void    GlobalConfig::PrintServerConfig(unsigned int index)
{
    if (index <= (_ServerCount - 1)) {
        std::cout << "server index              : " << index << "\n";
        std::cout << "host                      : " << _ServersConfigVec[index].GetHost() << "\n";
        std::cout << "server post               : " << _ServersConfigVec[index].GetPort() << "\n";
        if (_ServersConfigVec[index].GetServerName() != "n/a")
            std::cout << "server name               : " << _ServersConfigVec[index].GetServerName() << "\n";
        std::cout << "server client body size   : " << _ServersConfigVec[index].GetClientBodySize() << "\n";
        std::cout << "\nserver location(s)\n\n";
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
         std::cout << "location index            : " << index << "\n";
         std::cout << "location path             : " << _LocationsVec[index].GetLocationPath() << "\n";
         std::cout << "location root             : " << _LocationsVec[index].GetRoot() << "\n";
         std::cout << "location auto index       : " ;
         if (_LocationsVec[index].GetAutoIndex() == 0)
             std::cout << "OFF";
         else
             std::cout << "OFF";
         std::cout << "\n";
         if (_LocationsVec[index].GetCgiInfo().type != "n/a")
         {
             std::cout << "location cgi type         : " << _LocationsVec[index].GetCgiInfo().type << "\n";
             std::cout << "location cgi path         : " << _LocationsVec[index].GetCgiInfo().path << "\n";
         }
         if (_LocationsVec[index].GetUpload() != "n/a")
             std::cout << "server _Upload         : " << _LocationsVec[index].GetUpload() << "\n";
         if (_LocationsVec[index].GetAllowedMethodsVec().size() > 0)
         {
             std::cout << "location allowed methods  : " ;
             for (unsigned int i = 0; i < _LocationsVec[index].GetAllowedMethodsVec().size(); i++)
                 std::cout << _LocationsVec[index].GetAllowedMethodsVec()[i] << " ";
         }
         std::cout << "\n";
     }
 }

void    GlobalConfig::PrintServers(void)
{
    for (unsigned int i = 0; i < _ServersConfigVec.size(); i++)
        PrintServerConfig(i);
}

// --------------------- ACCESSOR ----------------
unsigned int GlobalConfig::GetServerCount(void)
{
    return (_ServerCount);
}

std::vector<ServerConfig>& GlobalConfig::GetServersVector(void)
{
    return (_ServersConfigVec);
}

unsigned int                    ServerConfig::GetPort(void)
{
    return _Port;    
}

std::string                     ServerConfig::GetHost(void)
{
    return _Host;
}

std::string                     ServerConfig::GetServerName(void)
{
    return _ServerName;
}

std::string                     ServerConfig::GetClientBodySize(void)
{
    return _ClientBodySize;
}
std::string                     ServerConfig::GetErrorPage(void)
{
    return  _ErrorPage;
}

std::vector<ServerLocation>&    ServerConfig::GetLocationsVec(void)
{
    return  _LocationsVec;
}

int ServerLocation::GetAutoIndex(void)
{
    return _AutoIndex;
}

cgi& ServerLocation::GetCgiInfo(void)
{
    return _CgiInfo;
}

std::string ServerLocation::GetLocationPath(void)
{
    return _LocationPath;
}

std::string ServerLocation::GetRoot(void)
{
    return _Root;
}

std::string ServerLocation::GetUpload(void)
{
    return _Upload;
}

std::vector<std::string> ServerLocation::GetAllowedMethodsVec(void)
{
    return _AllowedMethodsVec;
}