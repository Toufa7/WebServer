#include "../includes/Server.hpp"

void    Error(const char *msg)
{
    perror(msg);
    // exit(1);
}

Server::Server(ServerConfig &config)
{
    this->_config = config;
}

void    Server::Init()
{
    memset(&server_infos, 0, sizeof(server_infos));
    server_infos.ai_family      = AF_INET;
    server_infos.ai_protocol    = SOCK_STREAM;
    getaddrinfo(this->_config.GetHost().c_str(), std::to_string(this->_config.GetPort()).c_str(), &server_infos, &sinfo_ptr);
}

void    Server::CreateServer()
{
    Init();
    if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_protocol, 0)) == -1)
       Error("Error: Creating socket failed -> ");
    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(server_socket, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
       Error("Error: Binding failed -> ");
    if (listen(server_socket, FD_SETSIZE) == -1)
       Error("Error: Listening failed -> ");
}

void Server::SendResponseHeader(int clt_skt)
{
    char response_header[] = "HTTP/1.1 200 OK\r\n"
                                "Server: Allah Y7ssen L3wan\r\n"
                                "Content-Length: 82013359\r\n"
                                "Content-Type: video/mp4\r\n\r\n";
    if (send(clt_skt, response_header, strlen(response_header), 0) == -1)
        Error("Error (Send) -> ");
}

void Server::DropClient()
{
    close(active_clt);
    FD_CLR(active_clt, &readfds);
    FD_CLR(active_clt, &writefds);
    _clients.erase(itb);
    close(itb->GetCltFd());
}

int Server::AcceptAddClientToSet()
{
    int newconnection = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr);
    fcntl(newconnection, F_SETFL, O_NONBLOCK);
    if (newconnection == -1)
        Error("Error (Accept) -> ");
    _clients.push_back(Client(newconnection, open("/Users/otoufah/Desktop/Arsenal.mp4", O_RDONLY)));
    client_write_ready = false;
    FD_SET(_clients.back().GetCltSocket(), &readfds);
    FD_SET(_clients.back().GetCltSocket(), &writefds);
    if (_clients.back().GetCltSocket() > maxfds)
        maxfds = _clients.back().GetCltSocket();
    return (newconnection);
}


void    Server::ReadAndSend()
{
    // Continue running even if the write/send operation fails due to a closed/invalid socket
    // signal(SIGPIPE, SIG_IGN);
    bytesread = read(itb->GetCltFd(), buffer, sizeof(buffer));
    if (bytesread == -1)
        Error("Error (Read) -> ");
    bytessent = send(active_clt, buffer, bytesread, 0);
    if (bytessent == -1)
    {
        DropClient();
        Error("Error (Send) -> ");
    }
    if (bytessent == 0 || bytesread == 0)
    {
        DropClient();
    }
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
    int flag = 0; 
    CreateServer();
    SelectSetsInit();
    bytesreceived = 0;
    // Iterate over servers 
    while (TRUE)
    {
        tmpfdsread = readfds;
        tmpfdswrite = writefds;
        activity = select(maxfds + 1, &tmpfdsread, &tmpfdswrite, NULL, &timeout);
        if (activity == -1)
           Error("Error (Select) -> ");
        if (FD_ISSET(server_socket, &tmpfdsread))
        {
            client_socket = AcceptAddClientToSet();
        }

        // std::cout << "List Size -> " << _clients.size() << "\n";

        for (itb = _clients.begin(); itb != _clients.end(); itb++)
        {
            // Calling driver function in client handler that takes the requested data and the flag /
            active_clt = itb->GetCltSocket();
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
                    client_write_ready = true;
                }
            }

            if (FD_ISSET(active_clt, &tmpfdswrite) && client_write_ready)
            {
                itb->_client_hanlder.setConfig(this->_config);
                itb->_client_hanlder.Driver(requested_data);
            }
            flag++;
        }
    }
    close(server_socket);
}