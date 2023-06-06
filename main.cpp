#include "includes/Server.hpp"
#include "includes/Handler.hpp"
#include "includes/Config.hpp"

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