#include "Handler.hpp"
#include "Config.hpp"
#include "Shared.hpp"

class Client
{
    private:
        int     _fd;
        int     _socket;
    public:
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
        void SetSocket(int val)
        {
            this->_socket = val;
        }
        int GetCltSocket()
        {
            return (this->_socket);
        }
};
