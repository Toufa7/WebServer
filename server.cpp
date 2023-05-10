#include "server.h"

// Setuping and startig the server : Creating socket -> binding -> listening (Handling multiple clients)
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
        char                    requested_data[1024];

        void    Init()
        {
            memset(&server_infos, 0, sizeof(server_infos));
            server_infos.ai_family      = AF_INET;
            server_infos.ai_protocol    = SOCK_STREAM;
            getaddrinfo(LOCALHOST, PORT, &server_infos, &sinfo_ptr);
        }

        void    SendResponse()
        {
            // std::ifstream   input;
            // std::string     response;


            // input.open("homepage.html");
            // if (input.is_open())
            // {
            //     while(std::getline(input, response));
            //         input.close();
            // }
            std::string response = "HTTP/1.1 200 OK\r\n"
                                    "Server: Allah Y7ssen L3wan\r\n"
                                    "Content-Type: text/html\r\n\r\n"
                                    "<h1>Salaam!</h1>";
            
            if ((msg_sent = send(client_socket, response.c_str(), response.length(), 0)) < 0)
            {
                std::cerr << "Error : Sending failed\n";
                exit(1);
            }
        }
    
        void    GetRequest()
        {
            if ((msg_received = recv(client_socket, requested_data, sizeof(requested_data), 0 )) < 0)
            {
                std::cerr << "Error : Receiving failed\n";
                exit(1);
            }
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
            SendResponse();
            GetRequest();
            close(client_socket);
        }
}; 

// Parsing and extracting infos (url path, headers ...) from the request
class   Request : public Server
{

    public:
        std::string path;
        std::string method;
        std::map<std::string, std::string> request;

        void    PrintRequest(char *req)
        {
            std::cout << req;
        }
};

class   Response : public Server
{
    
};




int main()
{
    Server  WebServer;
    Request Parsing;

    WebServer.Start();
    Parsing.PrintRequest(WebServer.requested_data);

    return (0);
}