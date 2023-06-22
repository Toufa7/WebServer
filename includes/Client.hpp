#include "Handler.hpp"
#include "Config.hpp"
#include "Shared.hpp"

class Client
{
    private:
        int             _socket;
    public:
        Client() {};
        ServerConfig    _cltconfig;
        Handler         _client_handler;
        Client(int socket)
        {
            _socket = socket;
            _client_handler.client_socket = socket;
            _client_handler.setData();
        }
        int GetCltSocket()
        {
            return (this->_socket);
        }
};
