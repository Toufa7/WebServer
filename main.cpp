#include "includes/Server.hpp"
#include "includes/Handler.hpp"
#include "includes/Config.hpp"

/*
    Project structure needed
        - Driver function (Requested data , flagheader)
        - New client attribute called handler(getcltsokcet)
        - add client socket attribute in handler classs
        - A new array server object (ports, ...)

*/

int main(int ac, char **av)
{
    GlobalConfig        configuration;
    std::vector<Server> servers;
    //char conf[17] = "conf/config.conf";

    if (ac == 2)
        configuration.ParseConfigFile(av[1]);

    std::cout << "First CGI ->  " << configuration.GetServersVector()[0].GetLocationsVec()[0].GetCgiInfoPhp().path << std::endl;
    std::cout << "Second CGI -> " << configuration.GetServersVector()[0].GetLocationsVec()[0].GetCgiInfoPerl().path << std::endl;
    // else
    //     configuration.ParseConfigFile(conf);
    // for (size_t i = 0; i < configuration.GetServersVector().size(); i++)
    // {
    //     Server  WebServer(configuration.GetServersVector()[i]);
    //     WebServer.CreateServer();
    //     WebServer.SelectSetsInit();
    //     servers.push_back(WebServer);
    // }

    // while (TRUE)
    // {
    //     for (size_t i = 0; i < servers.size(); i++)
    //     {
    //         servers[i].Start();
    //     }
    // }
    return (0);
}