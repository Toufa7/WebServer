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
        //WebServer.Start();
    }
    else
    {
        std::cout << "Missing config file." << "\n";
        return (1);
    }
    return (0);
}