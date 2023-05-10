#include "server.hpp"

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
            char response_header[1024] = "HTTP/1.1 200 OK\r\n"
                                        "Server: Allah Y7ssen L3wan\r\n"
                                        "Content-Length: 68438\r\n"
                                        "Content-Type: image/png\r\n"
                                        "Connection: close\r\n\r\n";
            if (send(client_socket, response_header, strlen(response_header), 0) == -1)
            {   
                std::cerr << "Error : Receiving failed\n";
                exit(1);
            }
            int fd = open("./poms.jpeg", O_RDONLY);
            if (fd == -1)
            {
                std::cerr << "Error : Opening failed\n";
                exit(1);
            }
            char response_body[68438];
            int nbyte = read(fd, response_body, 68438);
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
            int optval = 1;
            /*
                * option to enable the reuse of local addresses /
                * used to set or get socket-level options that are not specific to any particular protocol or layer. /
                * socket-level options include
            */
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
        std::string status_line;
        std::map<std::string, std::string> request;
        std::string header;

        
        void    PrintRequest(char *req)
        {
            header = req;
            std::cout << header;
        }

        void  ParseRequest(char *req)
        {
            int delimiter_position;
            std::string current_line, key, value;
            std::stringstream request_stream(req);

            std::getline(request_stream, status_line); //status line
            while ( getline(request_stream >> std::ws >> std::skipws, current_line, '\n') )
            {
                current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end());    //remove every occurence of '/r' in line
                delimiter_position = current_line.find(':');
                key = current_line.substr(0, delimiter_position);                           
                value = current_line.substr(delimiter_position + 1, current_line.length());
                request[key] = value;   //storing key and value in map
                key.erase();    //erase key and value
                value.erase();  //for next iteration
            }
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
    //Parsing.PrintRequest(WebServer.requested_data);
    Parsing.ParseRequest(WebServer.requested_data);

    return (0);
}