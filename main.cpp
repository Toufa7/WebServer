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
    GlobalConfig    configuration;

    // Missing Config : Add Default Config 
    if (ac == 2)
    {
        configuration.ParseConfigFile(av[1]);
        Server          WebServer(configuration.GetServersVector()[0]);
        WebServer.Start();
    }
    else
        InvalidConfigFile("Missing config file.");
    return (0);
}