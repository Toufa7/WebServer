#include "../includes/Server.hpp"

//  ------------- CONSTRUCTOR && DESTRUCTOR --------------------

Server::Server(ServerConfig &config)
{
    this->_config = config;
}

// ------------- METHODS -------------

void    Server::Init()
{
    memset(&server_infos, 0, sizeof(server_infos));
    server_infos.ai_family      = AF_INET;
    server_infos.ai_protocol    = SOCK_STREAM;
    getaddrinfo(LOCALHOST, PORT, &server_infos, &sinfo_ptr);
}

// Reading the arriving request message from the client
void    Server::GetRequest()
{
    // requested_data = new char[10240];
    if ((msg_received = recv(client_socket, requested_data, sizeof(requested_data), 0 )) < 0)
    {
        std::cerr << "Error : Receiving failed\n";
        exit(1);
    }
    // Printing the request
    // std::cout << "--------------------------------------------------------------\n";
    // write(1, requested_data, msg_received);
}

void    Server::Start()
{
    Init();
    if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_protocol, 0)) == -1)
    {
        std::cerr << "Error: Creating socket failed\n";
        exit(1);
    }
    int optval = 1;
    // Allowing it to reuse the port instead of waiting
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(server_socket, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
    {
        std::cerr << "Error: Binding failed\n";
        exit(1);
    }
    if (listen(server_socket, BACKLOG) == -1)
    {
        std::cerr << "Error: Listening failed\n";
        exit(1);
    }

    clt_addr = sizeof(storage_sock);
    while (TRUE)
    {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr)) == -1)
        {
            std::cerr << "Error: Accepting failed\n";
            exit(1);
        }
        GetRequest();
        this->_config.setClientSocket(client_socket);
        this->_handler.setConfig(this->_config);
        this->_handler.ParseRequestHeader(requested_data);
    }
    close(client_socket);
}