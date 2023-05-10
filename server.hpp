#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <map>




#define PORT        "8080"
#define BACKLOG     10          // Max allowed connections can be queued up 
#define LOCALHOST   "127.0.0.1"



#endif 