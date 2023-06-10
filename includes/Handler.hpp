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

class Handler
{
public:

	void Driver(char *requested_data);

	void parseRequestHeader(char *req);

	std::string getRequestMethod();
	std::string getRequestURI();
	std::string getMimeType();
	void setConfig(ServerConfig &config);
	int getWorkingLocationIndex();
	void	sendResponseHeader(std::string statusCode, std::string fileExt, std::string location, int contentLength);
	void HandleGet(int headerflag);
	void HandleDelete();
	int	client_socket;
	int flaghead;
	int requested_file;

private:
	std::map<std::string, std::string> _req_header;
	std::string _method;
	std::string _uri; // The Request-URI (Uniform Resource Identifier)
	std::string _status_code;
	std::string _status_text;
	Shared _shared;
	ServerConfig _config;
	int _WorkingLocationIndex;




	void HandlePost(char *body);
	bool validateRequest();
	bool matchLocation();
	bool validateURI(const std::string &uri);
	void printRequstData();
	void sendErrorResponse(std::string statusCode);
	void redirectionResponse(std::string statusCode, std::string location);
	std::string generateListDir(std::string statusCode, std::string ls);
	void sendFileResponse(std::string statusCode, std::string path);
	void DeleteFile(const char *path);
	void DeleteDirectory(const char *path);
};

#endif