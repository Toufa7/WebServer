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
#include <sys/stat.h>
#include <fstream>

#include <map>
#include <cstring>
#include <sstream>
#include "Handler.hpp"
#include "Config.hpp"

#define PORT        "8080"
#define BACKLOG     10              // Max allowed connections can be queued up
#define LOCALHOST   "127.0.0.1"
#define TRUE        1
#define CRLF        "\r\n"          // The determination of the line 
#define MAX_CLT     10
#define CHUNK_SIZE  1024

// Setuping and startig the server : Creating socket -> binding -> listening (Handling multiple clients)
class Server
{
    public:
        Server();
        Server(ServerConfig &config);
        // Server Init
        int                     server_socket; // The server listen on this socket
        int                     client_socket; // The server serve the client on this socket
        struct addrinfo         server_infos;
        struct addrinfo         *sinfo_ptr;
        struct sockaddr_storage storage_sock;
        socklen_t               clt_addr;
        size_t                  msg_sent;
        size_t                  msg_received;

        // Multiplexing
        char                    requested_data[8192];
        int                     content_length;
        struct stat             infos;
        int                     fildes;
        char                    buffer[CHUNK_SIZE];
        int                     bytesread, bytessent,videosize, idx;
        fd_set                  readfds, tmpfds;
        int                     maxfds, activity, active_clt;
        int                     clients[MAX_CLT];


        


        // Member Functions
        void Init();
        void Start();
        void SendResponseHeader(int clt_skt);
        void CreateServer();
    
    private:
        ServerConfig _config;
        Handler _handler;
};

#endif