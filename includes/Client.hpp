#include "Handler.hpp"
#include "Config.hpp"
#include "Shared.hpp"

// Adding a handler object that takes the working client on

class Client
{
    private:
        int             _socket;
    public:
        // Constructor
        ServerConfig    _cltconfig;
        Handler _client_handler;
        Client() {};
        Client(int socket)
        {
            _socket = socket;
            _client_handler.client_socket = socket;
        }
        int GetCltSocket()
        {
            return (this->_socket);
        }
};
