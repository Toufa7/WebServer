#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <fstream>

#include <map>
#include <list>
#include <cstring>
#include <sstream>
#include "Handler.hpp"
#include "Config.hpp"
#include "Client.hpp"

#define BACKLOG     10              // Max allowed connections can be queued up
#define TRUE        1
#define FAILURE        0
#define CHUNK_SIZE  1024

// Setuping and startig the server : Creating socket -> binding -> listening (Handling multiple clients)


class Server
{
    private:
        ServerConfig                _config;
        std::list<Client>           _clients;
        std::list<Client>::iterator itb;

    public:
        Server(){};
        Server(ServerConfig &config);
        int                     server_socket; // The server listen on this socket
        int                     client_socket; // The server serve the client on this socket
        struct addrinfo         server_infos;
        struct addrinfo         *sinfo_ptr;
        struct sockaddr_storage storage_sock;
        socklen_t               clt_addr;
        char                    requested_data[CHUNK_SIZE];
        char                    buffer[CHUNK_SIZE];
        int                     bytesread, bytessent, bytesreceived;
        fd_set                  readfds, writefds, tmpfdsread, tmpfdswrite;
        int                     maxfds, activity, active_clt;
        bool                    readyforwrite;
        struct timeval          timeout;

    
        void    Init();
        void    Start();
        void    DropClient();
        void    CreateServer();
        void    SelectSetsInit();
        int     AcceptAddClientToSet();
};

#endif

