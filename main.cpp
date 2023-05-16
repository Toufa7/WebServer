#include "includes/Server.hpp"
#include "includes/Handler.hpp"
#include "includes/Config.hpp"


int main(int ac, char **av)
{
    Server          WebServer;
    Handler         Parsing;
    GlobalConfig    configuration;

    if (ac == 2)
    {
        configuration.parse_config_file(av[1]);
        configuration.print_server_config(0);
        std::cout << "locations count ---> " << configuration.servers[0]._locations.size() << "\n";
        //WebServer.Start();
    }
    else
    {
        std::cout << "Missing config file." << "\n";
        return (1);
    }
    return (0);
}