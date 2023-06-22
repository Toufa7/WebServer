#include "includes/Server.hpp"
#include "includes/Handler.hpp"
#include "includes/Config.hpp"

int main(int ac, char **av)
{
    // Seed the random number generator
    std::srand(time(NULL));
    
    std::vector<unsigned int> ports;
    std::vector<unsigned int>::iterator it;

    GlobalConfig        configuration;
    std::vector<Server> servers;
    char conf[17] = "conf/config.conf";

    if (ac == 2)
        configuration.ParseConfigFile(av[1]);
    else
        configuration.ParseConfigFile(conf);
    for (size_t i = 0; i < configuration.GetServersVector().size(); i++)
    {
        Server  WebServer(configuration.GetServersVector()[i]);
        WebServer.CreateServer();
        WebServer.SelectSetsInit();
        servers.push_back(WebServer);
        it = std::find(ports.begin(), ports.end(), configuration.GetServersVector()[i].GetPort());
        if (it != ports.end())
        {
            std::cout << "Already Used\n";
            exit(0);
        }
        ports.push_back(configuration.GetServersVector()[i].GetPort());
    }
    while (TRUE)
    {
        for (size_t i = 0; i < servers.size(); i++)
        {
            servers[i].Start();
        }
    }
    return (0);
}
