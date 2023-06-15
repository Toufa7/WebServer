#include "Handler.hpp"
#include "Config.hpp"
#include "Shared.hpp"

// Adding a handler object that takes the working client on

class Client
{
    private:
       // int     _fd;
        int             _socket;
    public:
        // Constructor
        ServerConfig    _cltconfig;
        Handler _client_handler;
        Client() {};
        Client(int socket)
        {
            _socket = socket;
            // _cltconfig = cltconfig;
            //_fd = fildes;
            _client_handler.client_socket = socket;
            _client_handler._postFileFd = -1;
            _client_handler.headerflag = 0;
        }
        // int GetCltFd()
        // {
        //     return (this->_fd);
        // }
        int GetCltSocket()
        {
            return (this->_socket);
        }
};
