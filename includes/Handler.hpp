#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <sys/socket.h>
#include <map>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <ios>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <dirent.h>
#include "Shared.hpp"
#include "Config.hpp"

#define CHUNK_SIZE 1024

class Handler
{
public:
	Handler();
	std::string GetRootLocation(std::string uri, std::string locationPath, std::string root);
	int Driver(char *requested_data, int bytesreceived);

	std::string getRequestMethod();
	std::string getRequestURI();
	void setConfig(ServerConfig &config);
	int client_socket;
	int _cgiTmpFilefd;
	std::string _cgiTmpFileName;
	std::map<std::string, std::string> _req_header;

private:
	std::string _method;
	std::string _uri; // The Request-URI (Uniform Resource Identifier)
	std::string _path;
	std::string _querystring;
	std::string _chunkHex;
	int _chunkHexState;
	Shared _shared;
	ServerConfig _config;
	ServerLocation _workingLocation;
	int requested_file;
	char buffer[CHUNK_SIZE];
	int bytesread, bytessent;
	int _postRecv, _chunkSize;
	int _postFileFd;
	int _headerflag;

	int parseRequestHeader(char *req, int bytesreceived);
	int HandlePost(char *body, int bytesreceived);
	int HandleGet();
	int HandleDelete();
	int GetSendFile();
	int HandleCgi(std::string path, std::string method, int header_flag);
	char **CgiSetEnv(std::string method);
	bool validateRequest();
	bool matchLocation();
	bool validateURI(const std::string &uri);
	void printRequstData();
	void sendResponseHeader(std::string statusCode, std::string fileExt, std::string location, int contentLength);
	void sendCodeResponse(std::string statusCode);
	std::string generateListDir(std::string statusCode, std::string ls);
	void DeleteFile(const char *path);
	void DeleteDirectory(const char *path);
};

#endif