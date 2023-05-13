#include "includes/Server.hpp"
#include "includes/Handler.hpp"

int main()
{
    Server  WebServer;
    Handler Parsing;

    WebServer.Start();
    return (0);
}