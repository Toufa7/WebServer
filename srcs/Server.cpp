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

    fildes = open("./test/homepage.html", O_RDONLY);
    if (fildes == -1)
        Error("Error (Open) -> ");
    fstat(fildes, &infos);
    videosize = infos.st_size;
}

void Server::Start()
{
    CreateServer();
    int maxfds, activity, active_clt;
    int clients[MAX_CLT];
    fd_set  readfds, tmpfds;
    struct timeval timeout;
    timeout.tv_sec = 100;
    timeout.tv_usec = 100;



    // Init our array of clients socket with -1
    for (int i = 0; i < MAX_CLT; i++)
        clients[i] = -1;
    
    // Zero the readfds
    FD_ZERO(&readfds);
    // Add the readfds to the set
    FD_SET(server_socket, &readfds);
    maxfds = server_socket;
    int bytesreceived = 0;

    while (true)
    {
        tmpfds = readfds;
        // Anything except -1 it's a success for select()
        std::cout << "Before Select \n";
        activity = select(maxfds + 1, &tmpfds, NULL, NULL, &timeout);
        std::cout << "After Select \n";
        if (activity == -1)
           Error("Error (Select) -> ");
        // checking for new connections need to be accepted        
        if (FD_ISSET(server_socket, &tmpfds))
        {
            if ((client_socket = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr)) == -1)
               Error("Error (Accept) -> ");
            // Find an empty slot in the set & add the new client_socket to the client socket set (Array)
            for (int i = 0; i < MAX_CLT; i++)
            {
                if (clients[i] < 0)
                {
                    clients[i] = client_socket;
                    break;
                }
            }
            // Add the new fildes to the set && update the max fildes
            FD_SET(client_socket, &tmpfds);
            if (client_socket > maxfds)
                maxfds = client_socket;
        }
        // Loop over the client array to handle active connection
        for (int i = 0; i < MAX_CLT; i++)
        {
            active_clt = clients[i];
            if (active_clt < 0)
                continue;
            if (FD_ISSET(active_clt, &tmpfds))
            {
                    bytesreceived = recv(active_clt, requested_data, sizeof(requested_data), 0);
                    // Closed connection from the client
                    if (bytesreceived == 0)
                    {
                        close(active_clt);
                        FD_CLR(active_clt, &tmpfds);
                        clients[i] = -1;
                        std::cout << "Connection Closed\n";
                    }
                    else if (bytesreceived < 0)
                    {
                        close(active_clt);
                        FD_CLR(active_clt, &tmpfds);
                        clients[i] = -1;
                        Error("Error (Recv) -> ");
                    }
                    // I know i should only send the header one time and as well opening the file and extract the infos
                    else 
                    {
                        std::cout << "------> Header \n";
                        char response_header[100] = "HTTP/1.1 200 OK\r\n"
                                                    "Server: Allah Y7ssen L3wan\r\n"
                                                    "Content-Length: 332\r\n"
                                                    "Content-Type: text/html\r\n\r\n";
                        if (send(active_clt, response_header, strlen(response_header), 0) == -1)
                            Error("Error: Sending Header Failed\n");
                    }
                    std::cout << "------> Body \n";;          
                    bytesread = read(fildes, buffer, sizeof(buffer));
                    std::cout << "Read \n" << bytesread << "\n";
                    if (bytesread == -1)
                        Error("Error (Read) -> ");
                    bytessent = send(active_clt, buffer, bytesread, 0);
                    if (bytessent == -1)
                        Error("Error (Send) -> ");
            }
        }
    }
    close(server_socket);
}