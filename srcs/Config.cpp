#include "../includes/Config.hpp"

ServerLocation::ServerLocation()
{
    //all ints shall be intialised by 0
    //and all strings by n/a 'not applicable'
    alwd_mtd.del = FALSE;
    alwd_mtd.get = FALSE;
    alwd_mtd.post = FALSE;

    cgi_info.type = "n/a";
    cgi_info.type = "n/a";

    location_path = "n/a";
    auto_index = FALSE;
    root = "n/a";
    upload = "n/a";   
}

ServerLocation::~ServerLocation()
{
    //ConfigLocation destructor
}

ServerConfig::ServerConfig()
{
    _port = 8080;
    _host = "n/a";
    _server_name = "n/a";
    _client_body_size = "n/a";
    _error_page = "n/a";   
}

ServerConfig::~ServerConfig()
{
    //config destructor
}

void GlobalConfig::parse_config_file(char *av)
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
        {
            std::cout << "Invalid config file: wrong extention." << "\n";
            return;
        }
    }
    else
    {
        std::cout << "Invalid config file: wrong extention." << "\n";
        return;
    }
    config_file.open(file_name);
    if (!config_file)
        std::cout << "There was an error when opening config file.";
    while (config_file)
    {
        std::getline(config_file, buffer);
        read_data += buffer += '\n';
    }
    read_data.pop_back();
    /*------------------------------------- End of reading -----------------------------------------*/


    /*----------------------------------- Extracting each "server" ----------------------------------*/
    oc = read_data.find("server ");
    while (oc >= 0)
    {
        soc = read_data.find("server ", oc + 1);
        if (soc > oc)//since oc is already bigger than 0,so for soc to be after oc, it should be be bigger 
        {
            server = read_data.substr(oc, soc - oc);
            oc = read_data.find("server ", soc + 1);
            parse_server_config(server);
        }
        if (soc < oc && soc > 0)
        {
            server = read_data.substr(soc, oc - soc);
            soc = read_data.find("server ", oc + 1);
            parse_server_config(server);
        }
        if (soc < 0 || oc < 0) // if soc < 0 that means theres only one server
        {
            if (soc > 0)
                server = read_data.substr(soc, read_data.length());
            else if (oc > 0)
                server = read_data.substr(oc, read_data.length());
            parse_server_config(server);
            break;
        }
        //std::cout << "oc is second find " << oc << "soc is " << soc << std::endl;
    }
    /*--------------------------------- End of extracting "server" ----------------------------------*/
}

void    GlobalConfig::parse_server_config(std::string server)
{
    int key_pos, colon_pos, value_pos, scolon_pos;
    std::string location;
    ServerConfig tmp;

    /*------------------------------- host and port -----------------------------------*/
    key_pos = server.find("listen ");
    if (key_pos >= 0)
    {
        colon_pos = server.find(":", key_pos);
        tmp._host = server.substr(key_pos + 7, (colon_pos - (key_pos + 7)));
        value_pos = server.find(";", colon_pos);
        tmp._port = std::atoi(server.substr(colon_pos + 1, (((value_pos - colon_pos) - 1))).c_str());
    }
    else
        std::cout << "Invalid config file : listen directive is not found." << "\n";
    /*------------------------------- End of host port --------------------------------*/

    /*---------------------------- server_name -----------------------------------*/
    key_pos = server.find("server_name ");
    if (key_pos > 0)
    {
        scolon_pos = server.find(";", (key_pos + 12));
        tmp._server_name = server.substr((key_pos + 12), scolon_pos - (key_pos + 12));
    }
    /*---------------------------- End of server_name --------------------------------*/

    /*--------------------------------- client body size -------------------------------------*/
    key_pos = server.find("client_body_size ");
    if (key_pos >= 0)
    {
        scolon_pos = server.find(";", (key_pos + 17));
        tmp._client_body_size = server.substr((key_pos + 17), scolon_pos - (key_pos + 17));
    }
    else
        std::cout << "Invalid config file : client body size is not found or not valid." << "\n";
    /*---------------------------- End of client body size -----------------------------------*/

    /*-------------------------------------- error page ---------------------------------------*/
    key_pos = server.find("error_page ");
    if (key_pos > 0)
    {
        scolon_pos = server.find(";", (key_pos + 11));
        tmp._error_page = server.substr((key_pos + 11), scolon_pos - (key_pos + 11));
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
                parse_server_location(location);
                break;
            }
            else
                break;
            key_pos = server.find("location ", key_pos + 1);
        }
    }
    else
        std::cout << "Invalid config file : location directive not found." << "\n";
    /*----------------------------------- End find locations -----------------------------------*/

    servers.push_back(tmp);
    exit(0);
}

void    GlobalConfig::parse_server_location(std::string location)
{
    int key_pos, value_pos;
    ServerLocation location_tmp;
    std::string tmp_str;

    /*------------------------------------- find location path -------------------------------------*/
    key_pos = location.find("location ");
    value_pos = location.find("{", key_pos + 1);
    location_tmp.location_path = location.substr((key_pos + 9), value_pos - (key_pos + 9));
    /*----------------------------------- end find location path -----------------------------------*/

    /*----------------------------------------- allowed methods ----------------------------------------*/
    key_pos = location.find("allow_methods ");
    if (key_pos > 0)
    {
        value_pos = location.find(";", key_pos + 1);
        //std::cout << key_pos << "   " << value_pos << "\n"; 
        tmp_str = location.substr((key_pos + 14), value_pos - (key_pos + 14));
        if (tmp_str.find("GET") >= 0)
            location_tmp.alwd_mtd.get = 1;
        if (tmp_str.find("POST") >= 0)
            location_tmp.alwd_mtd.post = 1;
        if (tmp_str.find("DELETE") >= 0)
            location_tmp.alwd_mtd.del = 1;
        tmp_str.erase();
    }
    else
        std::cout << "Invalid config file : allowed methods not found." << "\n";    
    /*------------------------------------- End of allowed methods -------------------------------------*/

    /*----------------------------------------- auto index ----------------------------------------*/
    if (key_pos > 0)
    {
        key_pos = location.find("autoindex ");
        value_pos = location.find(";", key_pos + 1);
        tmp_str = location.substr((key_pos + 10), value_pos - (key_pos + 10));
        if (tmp_str.find("on") >= 0)
            location_tmp.auto_index = 1;
    }
    else
        std::cout << "Invalid config file : auto index not found." << "\n"; 
    /*------------------------------------- End of auto index -------------------------------------*/

    /*------------------------------------- find root -------------------------------------*/
    key_pos = location.find("root ");
    if (key_pos >= 0)
    {
        value_pos = location.find(";", key_pos + 1);
        location_tmp.root = location.substr((key_pos + 5), value_pos - (key_pos + 5));
    }
    else
        std::cout << "Invalid config file : root not found." << "\n"; 

    /*----------------------------------- end find root -----------------------------------*/
    //ba9i 5asni n9ad cgi
    servers[server_index]._locations.push_back(location_tmp); 
}
