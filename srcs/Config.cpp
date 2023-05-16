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
    alwd_mtd.del = FALSE;
    alwd_mtd.get = FALSE;
    alwd_mtd.post = FALSE;

    cgi_info.type = "n/a";
    cgi_info.path = "n/a";

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

GlobalConfig::GlobalConfig()
{
    server_count = 0;
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
                parse_server_config(server);
            }
            if (soc < oc && soc > 0)
            {
                server = read_data.substr(soc, oc - soc);
                soc = read_data.find("server ", oc + 1);
                parse_server_config(server);
            }
            if (soc < 0 || oc < 0) // if soc < 0 that means there's only one server
            {
                if (soc > 0)
                    server = read_data.substr(soc, read_data.length());
                else if (oc > 0)
                    server = read_data.substr(oc, read_data.length());
                parse_server_config(server);//call to this f() to parse each server individually
                break;
            }
        }
    }
    else
        InvalidConfigFile("Invalid config file : server directive not found");
    /*--------------------------------- End of extracting "server" ----------------------------------*/
}

void    GlobalConfig::parse_server_config(std::string server)
{
    int key_pos, colon_pos, value_pos, scolon_pos;
    std::string location;
    ServerConfig tmp;

    server_count++;
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
        InvalidConfigFile("Invalid config file : listen directive is not found.");
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
        InvalidConfigFile("Invalid config file : client body size is not found or not valid.");
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
                tmp.parse_server_location(location);
            }
            else
                break;
            key_pos = server.find("location ", key_pos + 1);
        }
    }
    else
        InvalidConfigFile("Invalid config file : location directive not found.");
    /*----------------------------------- End find locations -----------------------------------*/

    servers.push_back(tmp);
}

void    ServerConfig::parse_server_location(std::string location)
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
        InvalidConfigFile("Invalid config file : allowed methods not found.");
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
        InvalidConfigFile("Invalid config file : auto index not found.");
    /*------------------------------------- End of auto index -------------------------------------*/

    /*------------------------------------- find root -------------------------------------*/
    key_pos = location.find("root ");
    if (key_pos >= 0)
    {
        value_pos = location.find(";", key_pos + 1);
        location_tmp.root = location.substr((key_pos + 5), value_pos - (key_pos + 5));
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
        location_tmp.cgi_info.type = tmp_str.substr(0, tmp_str.find(" ", 1));
        location_tmp.cgi_info.path = tmp_str.substr(tmp_str.find(" ") + 1, value_pos - (tmp_str.find(" ") + 1));
    }
    /*----------------------------------- end of find cgi -----------------------------------*/
    this->_locations.push_back(location_tmp);
}

void    GlobalConfig::print_server_config(unsigned int index)
{
    if (index <= (server_count - 1)) {
        std::cout << "server index              : " << index << "\n";
        std::cout << "host                      : " << servers[index]._host << "\n";
        std::cout << "server post               : " << servers[index]._port << "\n";
        if (servers[index]._server_name != "n/a")
            std::cout << "server name               : " << servers[index]._server_name << "\n";
        std::cout << "server client body size   : " << servers[index]._client_body_size << "\n";
        std::cout << "\nserver location(s)\n\n";
        for (unsigned long i = 0; i < servers[index]._locations.size(); i++) {
            servers[index].print_server_location(i);
            std::cout << "\n";
        }
    }
}

void ServerConfig::print_server_location(unsigned int index)
{
    if (index <= _locations.size())
    {
        std::cout << "location index            : " << index << "\n";
        std::cout << "location path             : " << _locations[index].location_path << "\n";
        std::cout << "location root             : " << _locations[index].root << "\n";
        std::cout << "location auto index       : " ;
        if (_locations[index].auto_index == 0)
            std::cout << "OFF";
        else
            std::cout << "OFF";
        std::cout << "\n";
        if (_locations[index].cgi_info.type != "n/a")
        {
            std::cout << "location cgi type         : " << _locations[index].cgi_info.type << "\n";
            std::cout << "location cgi path         : " << _locations[index].cgi_info.path << "\n";
        }
        if (_locations[index].upload != "n/a")
            std::cout << "server upload         : " << _locations[index].upload << "\n";
        std::cout << "location allowed methods  : " ;
        if (_locations[index].alwd_mtd.post == 1)
            std::cout << "POST";
        if (_locations[index].alwd_mtd.get == 1)
            std::cout << " GET ";
        if (_locations[index].alwd_mtd.del == 1)
            std::cout << "DELETE";
        std::cout << "\n";
    }
}

void    GlobalConfig::print_servers(void)
{
    for (unsigned int i = 0; i < servers.size(); i++)
        print_server_config(i);
}