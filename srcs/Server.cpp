#include "../includes/Server.hpp"

Server::Server(ServerConfig &config)
{
    this->_config = config;
}

void    Server::Init()
{
    std::cout << this->_config.GetHost().c_str() << ":" << std::to_string(this->_config.GetPort()).c_str()  << std::endl;
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
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
        perror("Error: FCNTL <Server Socket> -> ");
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
    // if (active_clt > 0)
    close(active_clt);
    // if (this->itb->_client_handler.requested_file > 0)
    close(this->itb->_client_handler.requested_file);
    FD_CLR(active_clt, &readfds);
    FD_CLR(active_clt, &writefds);
    _clients.erase(itb++);
    maxfds -= 1;
}

int Server::AcceptAddClientToSet()
{
    int newconnection = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr);
    if (fcntl(newconnection, F_SETFL, O_NONBLOCK) == -1)
        perror("Error: FCNTL <New Connection> -> ");
    if (newconnection == -1)
        perror("Error: ACCEPT <New Connection> -> ");
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
    timeout.tv_usec = 0;
    
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(server_socket, &readfds);
    FD_SET(server_socket, &writefds);
    maxfds = server_socket;
}


void Server::Start()
{
    signal(SIGPIPE, SIG_IGN);   
    tmpfdsread = readfds;
    tmpfdswrite = writefds;
    /*
        ! Select keeps waiting for an activity once a return check I/O
    */
    activity = select(maxfds + 1, &tmpfdsread, &tmpfdswrite, NULL, &timeout);
    if (activity == -1)
       perror("Error: Select Failed -> ");
    /* 
        ^ Catching an activity and accepting the new conenction 
        ^ Always true whenever a new connection came to the server
    */
    if (FD_ISSET(server_socket, &tmpfdsread))
    {
        client_socket = AcceptAddClientToSet();
    }
    for (itb = _clients.begin(); itb != _clients.end();)
    {
        // std::cout << "List Size -> " << _clients.size() << "    Max Fds -> " << maxfds << std::endl;
        bytesreceived = 0;
        active_clt = itb->GetCltSocket();
        /* 
            ? Socket is ready to read
        */
        // std::cout << "Activiy On Read -> " << FD_ISSET(active_clt, &tmpfdsread) << std::endl;
        if (FD_ISSET(active_clt, &tmpfdsread))
        {
            bytesreceived = recv(active_clt, requested_data, sizeof(requested_data), 0);
            if (bytesreceived == 0)
            {
                std::cerr << "Connection Closed by peer" << std::endl;
                DropClient();
                continue;
            }
            else if (bytesreceived < 0)
            {
                perror("Error: RECV <An error occured during the receiving> -> ");
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
        // std::cout << "Activiy On Write -> " << FD_ISSET(active_clt, &tmpfdswrite) << std::endl;
        if (FD_ISSET(active_clt, &tmpfdswrite) && readyforwrite == true)
        {
            if (itb->_client_handler.Driver(requested_data, bytesreceived) == 0)
            {
                DropClient();
                continue;
            }
        }
        itb++;
    }
}
