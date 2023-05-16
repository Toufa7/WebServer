#include "includes/Server.hpp"
#include "includes/Handler.hpp"
#include "includes/Config.hpp"


int main(int ac, char **av)
{
    GlobalConfig    configuration;

    if (ac == 2)
    {
        configuration.parse_config_file(av[1]);
        Server          WebServer(configuration.servers[0]);
        configuration.print_servers();
        WebServer.Start();
    }
    else
        InvalidConfigFile("Missing config file.");
    return (0);
}