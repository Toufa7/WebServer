#include "includes/Server.hpp"
#include "includes/Handler.hpp"

int main()
{
    Server  WebServer;
    Handler Parsing;

    WebServer.Start();
    Parsing.parseRequest(WebServer.requested_data);

    return (0);
}