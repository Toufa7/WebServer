#include "server.h"


class   Response
{
    
};

class   Request
{

};

class   Server
{
    public:
        int                     server_socket; // The server listen on this socket
        int                     client_socket; // The server serve the client on this socket
        struct addrinfo         server_infos;
        struct addrinfo         *sinfo_ptr;
        struct sockaddr_storage storage_sock;
        socklen_t               clt_addr;
        size_t                  msg_sent;
        size_t                  msg_received;

        void    Init()
        {
            memset(&server_infos, 0, sizeof(server_infos));
            server_infos.ai_family      = AF_INET;
            server_infos.ai_protocol    = SOCK_STREAM;
            getaddrinfo(LOCALHOST, PORT, &server_infos, &sinfo_ptr);
        }

        const char *Response()
        {
            const char* response = "HTTP/1.1 200 OK\r\n"
                                    "Server: Allah Y7ssen L3wan\r\n"
                                    "Content-Type: text/html\r\n\r\n"
                                    "<h1>Salaam!</h1>";
            return (response);
        }
    
        void    Request()
        {
            char req[1024];
            if ((msg_received = recv(client_socket, req, sizeof(req), 0 )) < 0)
            {
                std::cerr << "Error : Receiving failed\n";
                exit(1);
            }
            std::cout << req;
        }

        void    Start()
        {
            Init();

            if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_protocol, 0)) == -1)
            {
                std::cerr << "Error: Creating socket failed\n";
                exit(1);
            }

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
            if ((client_socket = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr)) == -1)
            {
                std::cerr << "Error: Accepting failed\n";
                exit(1);
            }


            if ((msg_sent = send(client_socket, Response(), strlen(Response()), 0)) < 0)
            {
                std::cerr << "Error : Sending failed\n";
                exit(1);
            }
            Request();
            close(client_socket);
        }
}; 

int main()
{
    Server WebServer;

    WebServer.Start();
    return (0);
}