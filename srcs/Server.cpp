#include "../includes/Server.hpp"

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
    // std::cout << "Host => " << this->_config.GetHost().c_str() << std::endl;
    // std::cout << "Port => " << std::to_string(this->_config.GetPort()).c_str() << std::endl;
    getaddrinfo(this->_config.GetHost().c_str(), std::to_string(this->_config.GetPort()).c_str(), &server_infos, &sinfo_ptr);
}

void    Server::CreateServer()
{
    Init();
    if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_socktype, sinfo_ptr->ai_protocol)) == -1)
       perror("Error: Socket failed -> ");
    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(server_socket, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
       perror("Error: Binding failed -> ");
    if (listen(server_socket, 10) == -1)
       perror("Error: Listening failed -> ");
}

void Server::DropClient()
{
    close(active_clt);
    FD_CLR(active_clt, &readfds);
    FD_CLR(active_clt, &writefds);
    _clients.erase(itb++);
}

int Server::AcceptAddClientToSet()
{
    int newconnection = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr);
    fcntl(newconnection, F_SETFL, O_NONBLOCK);
    if (newconnection == -1)
        perror("Error: Accepting Failed -> ");
    _clients.push_back(Client(newconnection));
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
        tmpfdsread = readfds;
        tmpfdswrite = writefds;
        /*
            ! Waiting for an activity
        */
        activity = select(maxfds + 1, &tmpfdsread, &tmpfdswrite, NULL, &timeout);
        if (activity == -1)
           perror("Error: Select Failed -> ");
        /* 
            ^ Catching an activity and Accepting the new conenction
        */
        if (FD_ISSET(server_socket, &tmpfdsread))
        {
            client_socket = AcceptAddClientToSet();
        }
        std::cout << "Currently working on => " << client_socket << std::endl;
        for (itb = _clients.begin(); itb != _clients.end();)
        {
            active_clt = itb->GetCltSocket();
            // Socket is ready for reading
            if (FD_ISSET(active_clt, &tmpfdsread))
            {
                bytesreceived = recv(active_clt, requested_data, sizeof(requested_data), 0);
                if (bytesreceived < 1)
                {
                    std::cerr << "Recv (-1) : Connection Closed -> " << active_clt << std::endl;
                    DropClient();
                    continue;
                }
                else
                {
                    readyforwrite = true;
                }
            }

            // Socket is ready for writing
            if (FD_ISSET(active_clt, &tmpfdswrite) && readyforwrite == true)
            {
                itb->_client_hanlder.setConfig(this->_config);
                itb->_client_hanlder.Driver(requested_data, bytesreceived);
            }
            itb++;
        }
    }
    close(server_socket);
}
