#include "Handler.hpp"
#include "Config.hpp"
#include "Shared.hpp"

// Adding a handler object that takes the working client on

class Client
{
    private:
       // int     _fd;
        int     _socket;
    public:
        // Constructor
        Handler _client_hanlder;
        Client() {};
        Client(int socket)
        {
            _socket = socket;
            //_fd = fildes;
            _client_hanlder.client_socket = socket;
            _client_hanlder._postFileFd = -1;
            _client_hanlder.headerflag = 0;
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
