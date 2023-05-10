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
#include <fstream>

#include <map>
#include <cstring>
#include <sstream>

#define PORT "8080"
#define BACKLOG 10 // Max allowed connections can be queued up
#define LOCALHOST "127.0.0.1"

// Setuping and startig the server : Creating socket -> binding -> listening (Handling multiple clients)
class Server
{
public:
    int server_socket; // The server listen on this socket
    int client_socket; // The server serve the client on this socket
    struct addrinfo server_infos;
    struct addrinfo *sinfo_ptr;
    struct sockaddr_storage storage_sock;
    socklen_t clt_addr;
    size_t msg_sent;
    size_t msg_received;
    char requested_data[1024];

    void Init();
    void SendResponse();
    void GetRequest();
    void Start();
};

#endif