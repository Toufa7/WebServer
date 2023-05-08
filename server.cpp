#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


#define PORT        8080
#define BACKLOG     10
#define LOCALHOST   "127.0.0.1"

int main()
{
    int socketfd;
    int clt_socket;
    struct addrinfo  server_infos, *sinfo_ptr;
    

    struct sockaddr_storage storage_sock;
    socklen_t clt_addr;


    server_infos.ai_family = AF_INET;
    server_infos.ai_protocol = SOCK_STREAM;
    
    getaddrinfo(LOCALHOST, "8080", &server_infos, &sinfo_ptr);

    if ((socketfd = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_protocol, 0)) == -1)
        std::cerr << "Error: Creating socket failed\n";

    if (bind(socketfd, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
        std::cerr << "Error: Binding failed\n";

    if (listen(socketfd, BACKLOG) == -1)
        std::cerr << "Error: Listening failed\n";

    clt_addr = sizeof(storage_sock);
    if ((clt_socket = accept(socketfd, (struct sockaddr *)&storage_sock, &clt_addr)) == -1)
        std::cerr << "Error: Listening failed\n";


    size_t msg_sent, msg_received;
    const char* response = "HTTP/1.1 200 OK\r\n"
                            "Server: Allah Y7ssen L3wan\r\n"
                            "Content-Type: text/html\r\n\r\n"
                            "<h1>Salaam!</h1>";
    if ((msg_sent = send(clt_socket, response, strlen(response), 0)) == -1)
        std::cerr << "Error : Sending failed\n";
    close(clt_socket);
    // if ((msg_received = recv(clt_socket, &ptr_msg, strlen(ptr_msg), 0)) == -1)
    //     std::cerr << "Error : Receving failed\n";

    return (0);
}
