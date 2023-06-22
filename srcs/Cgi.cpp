#include "../includes/Handler.hpp"

std::string CgiHeaderFindStatus(std::string &Header)
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
    std::string REDIRECT_STATUS = "REDIRECT_STATUS=200",
                REQUEST_METHOD = "REQUEST_METHOD=",
                SCRIPT_FILENAME = "SCRIPT_FILENAME=",
                PATH_INFO = "PATH_INFO=",
                CONTENT_TYPE = "CONTENT_TYPE=",
                CONTENT_LENGTH = "CONTENT_LENGTH=",
                HTTP_COOKIE= "HTTP_COOKIE=",
                QUERY_STRING = "QUERY_STRING=";
    REQUEST_METHOD += method;
    SCRIPT_FILENAME += this->_path.substr(_path.find_last_of('/', 0), _path.length());
    PATH_INFO += this->_path;
    if (this->_req_header.find("Content-Length") != this->_req_header.end())
        CONTENT_LENGTH += this->_req_header["Content-Length"];
    if (this->_req_header.find("Content-Type") != this->_req_header.end())
        CONTENT_TYPE += this->_req_header["Content-Type"];

	if (this->_req_header.find("Cookie") != this->_req_header.end())
        HTTP_COOKIE += this->_req_header["Cookie"];

    if (!this->_querystring.empty())
        QUERY_STRING += this->_querystring;

    char **newEnv = new char*[9];

    newEnv[0] = new char[REDIRECT_STATUS.length() + 1];
    newEnv[1] = new char[REQUEST_METHOD.length() + 1];
    newEnv[2] = new char[SCRIPT_FILENAME.length() + 1];
    newEnv[3] = new char[PATH_INFO.length() + 1];
    newEnv[4] = new char[QUERY_STRING.length() + 1];
    newEnv[5] = new char[CONTENT_TYPE.length() + 1];
    newEnv[6] = new char[CONTENT_LENGTH.length() + 1];
    newEnv[7] = new char[HTTP_COOKIE.length() + 1];

    memcpy(newEnv[0], REDIRECT_STATUS.c_str(), REDIRECT_STATUS.length() + 1);
    memcpy(newEnv[1], REQUEST_METHOD.c_str(), REQUEST_METHOD.length() + 1);
    memcpy(newEnv[2], SCRIPT_FILENAME.c_str(), SCRIPT_FILENAME.length() + 1);
    memcpy(newEnv[3], PATH_INFO.c_str(), PATH_INFO.length() + 1);
    memcpy(newEnv[4], QUERY_STRING.c_str(), QUERY_STRING.length() + 1);
    memcpy(newEnv[5], CONTENT_TYPE.c_str(), CONTENT_TYPE.length() + 1);
    memcpy(newEnv[6], CONTENT_LENGTH.c_str(), CONTENT_LENGTH.length() + 1);
    memcpy(newEnv[7], HTTP_COOKIE.c_str(), HTTP_COOKIE.length() + 1);
    newEnv[8] = NULL;
    
    return (newEnv);
}


int Handler::HandleCgi(std::string path, std::string method, cgi &cgitype)
{
    int outFd;
    std::string Buf, Header, Body, tmpfilename;
    struct stat s;

    if (stat(path.c_str(), &s) == 0 && (s.st_mode & S_IFREG))
    {
        if (this->_cgiPid == -1)
        {
            char **env = CgiSetEnv(method);

            tmpfilename = this->_shared.generateFileName("/tmp/", "");
            outFd = open(tmpfilename.c_str(), O_CREAT | O_RDWR | O_TRUNC , 0777);
            if (outFd < 0)
            {
                this->sendCodeResponse("500");
                return (0);
            }
            this->_cgiTmpFileName = tmpfilename;

            //PHP Script child process
            char *excearr[] = {const_cast<char *>(cgitype.path.c_str()), const_cast<char *>(path.c_str()), NULL};
            this->_cgiPid = fork();
            if (this->_cgiPid  == 0)
            {
                if (method == "POST")
                {
                    int fd = open(this->_postFilePath.c_str(), O_RDONLY);
                    dup2(fd, 0);
                    close(fd);
                }
                dup2(outFd, 1);
                close(outFd);
                execve(excearr[0], excearr, env);
            }
            // Free env
            for (unsigned int i = 0; env[i] != NULL; i++)
                delete env[i];
            delete env;
        }
        
        if (this->_cgiPid != -1 && waitpid(this->_cgiPid, NULL, WNOHANG) == this->_cgiPid)
        {
            //Reading tmp file
            std::ifstream TmpOutFile(this->_cgiTmpFileName.c_str());
            if (!TmpOutFile)
            {
                this->sendCodeResponse("500");
                return (0);
            }
            std::stringstream ss;
		    ss << TmpOutFile.rdbuf();
            Buf = ss.str();
            ss.clear();
            TmpOutFile.close();
            // Parse header
            Header = Buf.substr(0, Buf.find("\r\n\r\n"));
            if (Header.length() + 4 < Buf.length())
                Body = Buf.substr(Header.length() + 4, (Buf.length() - Header.length() - 4));
            Buf.clear();
            Buf.append(CgiHeaderFindStatus(Header));   
            Buf.append(Header);
            Buf.append(CgiBodySize(Body.length()));
            Buf.append("\r\n\r\n");
            Buf.append(Body);
            if (send(this->client_socket, Buf.c_str(), Buf.size(), 0) == -1)
                perror("Error : Send <Response Header>  -> ");
            remove(tmpfilename.c_str());
            if (method == "POST")
                remove(this->_postFilePath.c_str());
            this->_cgiPid = -1;

            return 0;
        }
        else if (this->_cgiPid != -1 && waitpid(this->_cgiPid, NULL, WNOHANG) == -1)
        {
            this->sendCodeResponse("500");
            return (0);
        }
    }
    else
    {
        this->sendCodeResponse("404");
        return (0);
    }
    return (1);
}
