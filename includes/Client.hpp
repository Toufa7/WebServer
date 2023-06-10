#include "Handler.hpp"
#include "Config.hpp"
#include "Shared.hpp"

// Adding a handler object that takes the working client on

class Client
{
    private:
        int     _fd;
        int     _socket;
    public:
        // Constructor
        Client() {};
        Client(int socket, int fildes)
        {
            _socket = socket;
            _fd = fildes;
        }
        Client(int socket)
        {
            _socket = socket;
        }
        int GetCltFd()
        {
            return (this->_fd);
        }
        int GetCltSocket()
        {
            return (this->_socket);
        }
};
