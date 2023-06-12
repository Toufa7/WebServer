#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>

std::string CgiHeaderFindStatus(std::string Header)
{
    int StatusPosition;
    std::string StatusString, StatusLocation, StatusCode;

    StatusPosition = Header.find("Status: ");
    if (StatusPosition < 0)
        return ("HTTP/1.1 200 OK\r\n");
    else
    {
        StatusCode = Header.substr(StatusPosition + 8, 3);
        StatusPosition = Header.find("Location: ");
        if (StatusPosition >= 0)
            StatusLocation = Header.substr(StatusPosition + 10, Header.find("\r\n", StatusPosition + 10));
        StatusString = "HTTP/1.1 ";
        StatusString += StatusCode;
        StatusString += " ";
        StatusString += StatusLocation;
    }
    StatusString += "\r\n";
    return (StatusString);
}

std::string CgiBodySize(int BodySize)
{
    std::string ContentLenght;
    struct stat FileStat;

    ContentLenght = "Content-Length: ";
    int StatReturn = stat("CGIout", &FileStat);
    if (StatReturn == 0)
        ContentLenght += std::to_string((BodySize));
    else
        ContentLenght += "0";
    ContentLenght += "\r\n";
    return (ContentLenght);
}

char **CgiSetEnv(char **oldEnv)
{
    std::string SERVER_PROTOCOL = "SERVER_PROTOCOL=HTTP/1.1",
                REDIRECT_STATUS = "REDIRECT_STATUS=200",
                GATEWAY_INTERFACE = "GATEWAY_INTERFACE=CGI/1.1",
                SCRIPT_NAME = "SCRIPT_NAME=Script.php",
                REQUEST_METHOD = "REQUEST_METHOD = GET";
                
                // SCRIPT_FILENAME = "",
                // PATH_INFO = "",
                // QUERY_STRING = "",
                
                //For Post only
                //CONTENT_TYPE = "CONTENT_TYPE=",
                //CONTENT_LENGTH = "CONTENT_LENGTH=";

    char **newEnv = new char*[5];
    
    newEnv[0] = new char[SERVER_PROTOCOL.length()];
    newEnv[1] = new char[REDIRECT_STATUS.length()];
    newEnv[2] = new char[GATEWAY_INTERFACE.length()];
    newEnv[3] = new char[SCRIPT_NAME.length()];
    newEnv[4] = new char[REQUEST_METHOD.length()];

    memcpy(newEnv[0], SERVER_PROTOCOL.c_str(), SERVER_PROTOCOL.length());
    memcpy(newEnv[1], REDIRECT_STATUS.c_str(), REDIRECT_STATUS.length());
    memcpy(newEnv[2], GATEWAY_INTERFACE.c_str(), GATEWAY_INTERFACE.length());
    memcpy(newEnv[3], SCRIPT_NAME.c_str(), SCRIPT_NAME.length());
    memcpy(newEnv[4], REQUEST_METHOD.c_str(), REQUEST_METHOD.length());
    newEnv[5] = NULL;

    return (newEnv);
}

int main(int ac, char **av, char **env)
{

    char **newEnv = CgiSetEnv(env);
    // for (int i = 0; newEnv[i] != NULL; i++)
    //     std::cout << "Env[" << i <<"] : " << newEnv[i] << '\n';

    //system("leaks a.out");
    int outFd, OutFdOpen, Status_pos, BodyPos;
    std::string Buf, TmpStr, Header, Body;
    std::ifstream TmpOutFile;
    std::stringstream Response;

    outFd = open("CGIout", O_CREAT | O_RDWR | O_TRUNC , 0777);
    char *excearr[] = {"php-cgi", "script.php", NULL};
    
    pid_t CID = fork();
    if (CID == 0)
    {
        dup2(outFd, 1);
        dup2(outFd, 2);
        close(outFd);
        execve(excearr[0], excearr, newEnv);
    }

    wait(NULL);

    TmpOutFile.open("CGIout");
    if (!TmpOutFile)
        std::cout << "The file could not be opened properly\n";
    while ( TmpOutFile )
    {
        std::getline(TmpOutFile, TmpStr);
        Buf += TmpStr += '\n';
    }
    TmpOutFile.close();

    Header = Buf.substr(0, Buf.find("\r\n\r\n"));
    BodyPos = Buf.find("<!DOCTYPE html>");
    if (BodyPos >= 0)
        Body = Buf.substr(BodyPos, (Buf.length() - BodyPos));
    std::cout << Body;

    Header += '\n';
    Header.insert(0, CgiHeaderFindStatus(Header));
    Header.insert(Header.length(), CgiBodySize(Body.length()));
    
   //std::cout << Header;
}

// SERVER_PROTOCOL=HTTP/1.1
// REDIRECT_STATUS=200
// GATEWAY_INTERFACE=CGI/1.1
// SCRIPT_NAME=
// SCRIPT_FILENAME=
// PATH_INFO=
// QUERY_STRING=
// REQUEST_METHOD=
// HTTP_COOKIE=" + client.parsedRequest.getValueFromMap("Cookie"))
