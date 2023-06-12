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
	std::string GetRootLocation(std::string uri, std::string locationPath, std::string root);
	int Driver(char *requested_data, int bytesreceived);


	std::string getRequestMethod();
	std::string getRequestURI();
	void setConfig(ServerConfig &config);
	int headerflag;
	int client_socket;
	int _postFileFd;
	std::map<std::string, std::string> _req_header;

private:
	std::string _method;
	std::string _uri; // The Request-URI (Uniform Resource Identifier)
	std::string _path;
	Shared _shared;
	ServerConfig _config;
	ServerLocation _workingLocation;
	int requested_file;
	char buffer[CHUNK_SIZE];
	int bytesread, bytessent;

	int parseRequestHeader(char *req, int bytesreceived);
	int HandlePost(char *body, int bytesreceived);
	int HandleGet();
	int HandleDelete();
	bool validateRequest();
	bool matchLocation();
	bool validateURI(const std::string &uri);
	void printRequstData();
	void sendResponseHeader(std::string statusCode, std::string fileExt, std::string location, int contentLength);
	void sendErrorResponse(std::string statusCode);
	std::string generateListDir(std::string statusCode, std::string ls);
	void DeleteFile(const char *path);
	void DeleteDirectory(const char *path);
};

#endif