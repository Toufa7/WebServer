#include "includes/Server.hpp"
#include "includes/Handler.hpp"

int main()
{
    Server  WebServer;
    Handler Parsing;

    WebServer.Start();
    Parsing.ParseRequest(WebServer.requested_data);

    return (0);
}