#include "../includes/Server.hpp"

#define FAILURE 0

Server::Server(ServerConfig &config)
{
    this->_config = config;
}

void    Server::Init()
{
    memset(&server_infos, 0, sizeof(server_infos));
    server_infos.ai_family      = AF_INET;
    server_infos.ai_socktype    = SOCK_STREAM;
    server_infos.ai_flags       = AI_PASSIVE;
    getaddrinfo(this->_config.GetHost().c_str(), std::to_string(this->_config.GetPort()).c_str(), &server_infos, &sinfo_ptr);
}

void    Server::CreateServer()
{
    Init();
    if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_socktype, sinfo_ptr->ai_protocol)) == -1)
       perror("Error: SOCKET failed -> ");
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
       perror("Error: SETSOCKOPT failed -> ");
    if (bind(server_socket, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
       perror("Error: BIND failed -> ");
    if (listen(server_socket, FD_SETSIZE) == -1)
       perror("Error: LISTEN failed -> ");
    freeaddrinfo(sinfo_ptr);
}

void Server::DropClient()
{
    close(active_clt);
    FD_CLR(active_clt, &readfds);
    FD_CLR(active_clt, &writefds);
    _clients.erase(itb++);
    maxfds -= 1;
}

int Server::AcceptAddClientToSet()
{
    int newconnection = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr);
    fcntl(newconnection, F_SETFL, O_NONBLOCK);
    if (newconnection == -1)
        perror("Error: ACCEPT Failed -> ");
    _clients.push_back(Client(newconnection));
    _clients.back()._client_handler.setConfig(this->_config);
    readyforwrite = false;
    FD_SET(_clients.back().GetCltSocket(), &readfds);
    FD_SET(_clients.back().GetCltSocket(), &writefds);
    if (_clients.back().GetCltSocket() > maxfds)
        maxfds = _clients.back().GetCltSocket();
    return (newconnection);
}

void Server::SelectSetsInit()
{
    timeout.tv_sec  = 100;
    timeout.tv_usec = 100;
    
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(server_socket, &readfds);
    FD_SET(server_socket, &writefds);
    maxfds = server_socket;
}


void Server::Start()
{
    CreateServer();
    SelectSetsInit();
    bytesreceived = 0;
    while (TRUE)
    {
        // signal(SIGPIPE, SIG_IGN);
        // std::cout << "Max fd-> " << maxfds << std::endl;
        tmpfdsread = readfds;
        tmpfdswrite = writefds;
        /*
            ! Select keeps waiting for an activity once a return check I/O
        */
        activity = select(maxfds + 1, &tmpfdsread, &tmpfdswrite, NULL, &timeout);
        if (activity == -1)
           perror("Error: Select Failed -> ");
        /* 
            ^ Catching an activity and Accepting the new conenction 
            ^ Always true whenever a new connection came to the server
        */
        if (FD_ISSET(server_socket, &tmpfdsread))
        {
            client_socket = AcceptAddClientToSet();
        }
        // std::cout << "List Size -> " << _clients.size() << std::endl;
        for (itb = _clients.begin(); itb != _clients.end();)
        {
            bytesreceived = 0;
            active_clt = itb->GetCltSocket();
            /* 
                ? Socket is ready to read
            */
            // bzero(requested_data, CHUNK_SIZE);
            if (FD_ISSET(active_clt, &tmpfdsread))
            {
                bytesreceived = recv(active_clt, requested_data, sizeof(requested_data), 0);
                if (bytesreceived == 0)
                {
                    std::cerr << "Connection Closed by peer (No more data will be received)" << std::endl;
                    DropClient();
                    continue;
                }
                else if (bytesreceived < 0)
                {
                    perror("Error: Recv failed -> ");
                    DropClient();
                    continue;
                }
                else
                {
                    readyforwrite = true;
                }
            }
            /* 
                ~ Socket is ready to write
            */
            if (FD_ISSET(active_clt, &tmpfdswrite) && readyforwrite == true && bytesreceived > 0)
            {
                // Connection Keep-Alive 
                if (itb->_client_handler.Driver(requested_data, bytesreceived) == FAILURE)
                {
                    DropClient();
                    continue;
                }
            }
            itb++;
        }
    }
    close(server_socket);
}