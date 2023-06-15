#include "../includes/Handler.hpp"

//TO DO

//client should save the opened tmp file fd : done
//content lenght POST : need to be tested cause it may not be necessary
//body size
//leaks:done
//remove cgi file when done:done
//check sys calls for error returns

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
            StatusLocation = Header.substr(StatusPosition + 10, (Header.find("\r\n", StatusPosition + 11) - (StatusPosition + 10)));
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
    //cgi body size
    std::string ContentLenght;
    // struct stat FileStat;

    ContentLenght = "Content-Length: ";
    // int StatReturn = stat(tmpfilename.c_str(), &FileStat);
    // if (StatReturn == 0)
    ContentLenght += std::to_string((BodySize));
    // else
    //     ContentLenght += "0";
    ContentLenght += "\r\n";
    
    return (ContentLenght);
}

char **Handler::CgiSetEnv(std::string method)
{
    std::string SERVER_PROTOCOL = "SERVER_PROTOCOL=HTTP/1.1",
                REDIRECT_STATUS = "REDIRECT_STATUS=200",
                GATEWAY_INTERFACE = "GATEWAY_INTERFACE=CGI/1.1",
                SCRIPT_NAME = "SCRIPT_NAME=",
                REQUEST_METHOD = "REQUEST_METHOD=",
                SCRIPT_FILENAME = "SCRIPT_FILENAME=",
                PATH_INFO = "PATH_INFO=",
                QUERY_STRING = "QUERY_STRING=";
                //For Post only
                //CONTENT_TYPE = "CONTENT_TYPE=",
                //CONTENT_LENGTH = "CONTENT_LENGTH=";
    REQUEST_METHOD += method;
    SCRIPT_FILENAME += this->_path.substr(_path.find_last_of('/', 0), _path.length());
    SCRIPT_NAME += this->_path.substr(_path.find_last_of('/', 0), _path.length());
    PATH_INFO += this->_path;

    if (!this->_querystring.empty())
        QUERY_STRING += this->_querystring;
    char **newEnv = new char*[9];

    newEnv[0] = new char[SERVER_PROTOCOL.length()];
    newEnv[1] = new char[REDIRECT_STATUS.length()];
    newEnv[2] = new char[GATEWAY_INTERFACE.length()];
    newEnv[3] = new char[SCRIPT_NAME.length()];
    newEnv[4] = new char[REQUEST_METHOD.length()];
    newEnv[5] = new char[SCRIPT_FILENAME.length()];
    newEnv[6] = new char[PATH_INFO.length()];
    newEnv[7] = new char[QUERY_STRING.length()];

    memcpy(newEnv[0], SERVER_PROTOCOL.c_str(), SERVER_PROTOCOL.length());
    memcpy(newEnv[1], REDIRECT_STATUS.c_str(), REDIRECT_STATUS.length());
    memcpy(newEnv[2], GATEWAY_INTERFACE.c_str(), GATEWAY_INTERFACE.length());
    memcpy(newEnv[3], SCRIPT_NAME.c_str(), SCRIPT_NAME.length());
    memcpy(newEnv[4], REQUEST_METHOD.c_str(), REQUEST_METHOD.length());
    memcpy(newEnv[5], SCRIPT_FILENAME.c_str(), SCRIPT_FILENAME.length());
    memcpy(newEnv[6], PATH_INFO.c_str(), PATH_INFO.length());
    memcpy(newEnv[7], QUERY_STRING.c_str(), QUERY_STRING.length());
    newEnv[8] = NULL;

    return (newEnv);
}

int Handler::HandleCgi(std::string path, std::string method, int header_flag)
{
    static int var = 0;
    var++;
    int outFd, BodyPos;
    std::string Buf, TmpStr, Header, Body, tmpfilename;
    std::ifstream TmpOutFile;
    std::ofstream ParsedTmpOutFile;
    std::stringstream TmpOutFileStream;
    struct stat s;

    TmpOutFile.setf(std::ios::binary);
    if (stat(path.c_str(), &s) == 0 && (s.st_mode & S_IFREG))
    {
        std::cout << "Var -> " << var << std::endl;
        if (header_flag == 0)
        {
            char **newEnv = CgiSetEnv(method); 
            tmpfilename = "tmpfiles/";
            tmpfilename = this->_shared.generateFileName("tmpfiles/", ".tmp");
            outFd = open(tmpfilename.c_str(), O_CREAT | O_RDWR | O_TRUNC , 0777);
            if (outFd < 0)
                this->sendCodeResponse("500");
            //this may cause back and forth calling if you add return
            this->_cgiTmpFileName = tmpfilename;
            
            //PHP Script child process
            char *excearr[] = {const_cast<char *>(this->_workingLocation.GetCgiInfo().path.c_str()), const_cast<char *>(path.c_str()), NULL};
            pid_t CID = fork();
            if (CID == 0)
            {
                dup2(outFd, 1);
                dup2(outFd, 2);
                close(outFd);
                execve(excearr[0], excearr, newEnv);
            }
            wait(0);
            
            //Free env
            for (unsigned int i = 0; newEnv[i] != NULL; i++)
                delete newEnv[i];
            delete newEnv;
            
            //Parsing CGI header
            TmpOutFile.open(this->_cgiTmpFileName.c_str());
            if (!TmpOutFile)
                std::cout << "The file could not be opened properly\n";
            while (TmpOutFile)
            {
                std::getline(TmpOutFile, TmpStr);
                Buf += TmpStr += '\n';
            }
            TmpOutFile.close();

            //Parse header
            Header = Buf.substr(0, Buf.find("\r\n\r\n"));
            TmpOutFileStream << CgiHeaderFindStatus(Header);
            TmpOutFileStream << Header;

            BodyPos = Buf.find("<!DOCTYPE html>");
            if (BodyPos > 0)
            {
                Body = Buf.substr(BodyPos, (Buf.length() - BodyPos));        
                TmpOutFileStream << CgiBodySize(Body.length());
            }

            ParsedTmpOutFile.open(this->_cgiTmpFileName);
            ParsedTmpOutFile << TmpOutFileStream.str();
            ParsedTmpOutFile << "\r\n\r\n";
            if (BodyPos >= 0)
                ParsedTmpOutFile << Body;
            ParsedTmpOutFile.close();
    
            this->_cgiTmpFilefd = open(this->_cgiTmpFileName.c_str(), O_RDONLY , 0777);
            bytesread = read(this->_cgiTmpFilefd, buffer, sizeof(buffer));
            if (bytesread == -1)
                perror("Error (Read) -> ");
            bytessent = send(this->client_socket, buffer, bytesread, 0);
            if (bytessent == -1 || bytessent == 0 || bytesread < CHUNK_SIZE)
            {
                perror("Error (Send : CGI Header) -> ");
                close(this->_cgiTmpFilefd);
                remove(tmpfilename.c_str());
                return (0);
            }
        }
        if (header_flag != 0)
        {
            bytesread = read(this->_cgiTmpFilefd, buffer, sizeof(buffer));
            if (bytesread == -1)
                perror("Error (Read) -> ");
            bytessent = send(this->client_socket, buffer, bytesread, 0);
            if (bytessent == -1 || bytessent == 0 || bytesread < CHUNK_SIZE)
            {
                perror("Error (Send : CGI Body) -> ");
                close(this->_cgiTmpFilefd);
                remove(tmpfilename.c_str());
                return (0);
            }
        }
    }
    else
        this->sendCodeResponse("404");
    return (1);
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
