#include "../includes/Server.hpp"

//  ------------- CONSTRUCTOR && DESTRUCTOR --------------------

void    Error(const char *msg)
{
    perror(msg);
    exit(1);
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
    getaddrinfo(LOCALHOST, PORT, &server_infos, &sinfo_ptr);
}

void    Server::CreateServer()
{
    Init();
    if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_protocol, 0)) == -1)
       Error("Error: Creating socket failed\n");
    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(server_socket, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
       Error("Error: Binding failed\n");
    if (listen(server_socket, BACKLOG) == -1)
       Error("Error: Listening failed\n");


}

void Server::SendResponseHeader(int clt_skt)
{
    char response_header[100] = "HTTP/1.1 200 OK\r\n"
                                "Server: Allah Y7ssen L3wan\r\n"
                                "Content-Length: 332\r\n"
                                "Content-Type: text/html\r\n\r\n";
    if (send(clt_skt, response_header, strlen(response_header), 0) == -1)
        Error("Error (Send) -> ");
}


void Server::Start()
{
    CreateServer();
    int maxfds, activity, active_clt;
    struct timeval timeout;
    timeout.tv_sec = 100;
    timeout.tv_usec = 100;
    
    // Zero the readfds
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    // Add the readfds to the set
    FD_SET(server_socket, &readfds);
    FD_SET(server_socket, &writefds);
    maxfds = server_socket;
    int bytesreceived = 0;

    while (TRUE)
    {
        // GetRequest();
        tmpfdsread = readfds;
        tmpfdswrite = writefds;
        activity = select(maxfds + 1, &tmpfdsread, &tmpfdswrite, NULL, &timeout);
        if (activity == -1)
           Error("Error (Select) -> ");
        if (FD_ISSET(server_socket, &tmpfdsread))
        {
            if ((client_socket = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr)) == -1)
               Error("Error (Accept) -> ");            
            _clients.push_back(Client(client_socket, open("/Users/ibnada/Desktop/trim.DD9DF7D5-FA6A-439E-9F1E-A85F35E2E9C8.mp4", O_RDONLY)));
                client_write_ready = false;
            FD_SET(_clients.back().GetCltSocket(), &readfds);
            FD_SET(_clients.back().GetCltSocket(), &writefds);
            if (_clients.back().GetCltSocket() > maxfds)
                maxfds = _clients.back().GetCltSocket();
        }
        for (itb = _clients.begin(); itb != _clients.end(); itb++)
        {
            active_clt = itb->GetCltSocket();
            //this->_config.setClientSocket(itb->GetCltSocket());
            //this->_handler.setConfig(this->_config);
            if (FD_ISSET(active_clt, &tmpfdsread) && !client_write_ready)
            {
                bytesreceived = recv(active_clt, requested_data, sizeof(requested_data), 0);
                //this->_handler.ParseRequestHeader(requested_data);
                if (bytesreceived == 0)
                {
                    close(active_clt);
                    FD_CLR(active_clt, &readfds);
                    _clients.erase(itb);
                    close(itb->GetCltFd());
                    std::cerr << "Connection Closed\n";
                }
                else if (bytesreceived < 0)
                {
                    close(active_clt);
                    FD_CLR(active_clt, &readfds);
                    _clients.erase(itb);
                    close(itb->GetCltFd());
                    Error("Error (Recv) -> ");
                }
                else 
                    SendResponseHeader(active_clt);
                client_write_ready = true;
            }
            if (FD_ISSET(active_clt, &tmpfdswrite) && client_write_ready)
            {
                bytesread = read(itb->GetCltFd(), buffer, sizeof(buffer));
                if (bytesread == -1)
                    Error("Error (Read) -> ");
                bytessent = send(active_clt, buffer, bytesread, 0);
                if (bytessent == -1)
                    Error("Error (Send) -> ");
            }
        }
        //  
    }
    close(server_socket);
}