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
                                "Content-Length: 82013359\r\n"
                                "Content-Type: video/mp4\r\n\r\n";
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



    // Init our array of clients socket with -1
    for (int i = 0; i < MAX_CLT; i++)
        clients[i] = -1;
    
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
        tmpfdsread = readfds;
        tmpfdswrite = writefds;
        // Anything except -1 it's a success for select()
        // std::cout << "Select listen for an activity ...\n";
        activity = select(maxfds + 1, &tmpfdsread, &tmpfdswrite, NULL, &timeout);
        // std::cout << "Select catch one \n";
        if (activity == -1)
           Error("Error (Select) -> ");
        // checking for new connections need to be accepted        
        if (FD_ISSET(server_socket, &tmpfdsread))
        {
            if ((client_socket = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr)) == -1)
               Error("Error (Accept) -> ");
            std::cout << "Cliuen FD -> " << client_socket << "\n";
            // Find an empty slot in the set & add the new client_socket to the client socket set (Array)
            for (int i = 0; i < MAX_CLT; i++)
            {
                if (clients[i] < 0)
                {
                    std::cout << "Clietn\n";
                    clients[i] = client_socket;
                    fildes[i] = open("/Users/otoufah/Desktop/Arsenal.mp4", O_RDONLY);
                    if (fildes[i] == -1)
                        Error("Error (Open) -> ");
                    client_write_ready = false;
                    break;
                }
            }
            // Add the new fildes to the set && update the max fildes
            FD_SET(client_socket, &readfds); // (a savoir)
            FD_SET(client_socket, &writefds);
            if (client_socket > maxfds)
                maxfds = client_socket;
        }
        // Loop over the client array to handle active connection
        for (int i = 0; i < MAX_CLT; i++)
        {
            active_clt = clients[i];
            if (active_clt < 0)
                continue;
            if (FD_ISSET(active_clt, &tmpfdsread) && !client_write_ready)
            {
                    std::cout << "Hello From read" << std::endl;
                    bytesreceived = recv(active_clt, requested_data, sizeof(requested_data), 0);
                    // Closed connection from the client
                    if (bytesreceived == 0)
                    {
                        close(active_clt);
                        FD_CLR(active_clt, &readfds); // (a savoir)
                        clients[i] = -1;
                        close(fildes[i]);
                        std::cout << "Connection Closed\n";
                    }
                    else if (bytesreceived < 0)
                    {
                        close(active_clt);
                        FD_CLR(active_clt, &readfds);
                        clients[i] = -1;
                        Error("Error (Recv) -> ");
                    }
                    // I know i should only send the header one time and as well opening the file and extract the infos
                    else 
                    {
                        SendResponseHeader(active_clt);
                    }
                    client_write_ready = true;
            }
            if (FD_ISSET(active_clt, &tmpfdswrite) && client_write_ready)
            {
                bytesread = read(fildes[i], buffer, sizeof(buffer));
                // std::cout << " -> " << bytesread << std::endl;
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