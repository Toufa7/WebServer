#include "../includes/Server.hpp"

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
    if ((msg_received = recv(client_socket, requested_data, sizeof(requested_data), 0 )) < 0)
    {
        std::cerr << "Error : Receiving failed\n";
        exit(1);
    }
    write(1, requested_data, msg_received);
}

void    Server::SendResponse()
{
    std::string response_header = "HTTP/1.1 200 OK\r\n"
                                "Server: Allah Y7ssen L3wan\r\n"
                                "Content-Length: 332\r\n"
                                "Content-Type: text/html\r\n"
                                "Connection: close\r\n\r\n";
    if (send(client_socket, response_header.c_str(), response_header.length(), 0) == -1)
    {   
        std::cerr << "Error : Receiving failed\n";
        exit(1);
    }
    int fd = open("test/homepage.txt", O_RDONLY);
    struct stat file_infos;
    if (fd == -1)
    {
        std::cerr << "Error : Opening failed\n";
        exit(1);
    }
    fstat(fd, &file_infos);
    char *response_body = new char[file_infos.st_size];
    int nbyte = read(fd, response_body, file_infos.st_size);
    if (nbyte == -1)
    {
        std::cerr << "Error : Reading failed\n";
        exit(1);
    }
    if (send(client_socket, response_body, nbyte, 0) == -1)
    {   
        std::cerr << "Error : Receiving failed\n";
        exit(1);
    }
    delete [] response_body;
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
        {
            // 
        }
        SendResponse();
    }
    close(client_socket);
}