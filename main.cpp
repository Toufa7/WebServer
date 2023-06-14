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
    char conf[15] = "conf/file.conf";

    if (ac == 2)
        configuration.ParseConfigFile(av[1]);
    else
        configuration.ParseConfigFile(conf);
    for (size_t i = 0; i < configuration.GetServersVector().size(); i++)
    {
        Server          WebServer(configuration.GetServersVector()[i]);
        WebServer.Start();
        servers.push_back(WebServer);
    }
    return (0);
}