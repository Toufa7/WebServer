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
		std::string	GetRootLocation(std::string uri, std::string locationPath, std::string root);
		int			Driver(char *requested_data, int bytesreceived);
		std::string	getRequestMethod();
		std::string	getRequestURI();
		void		setConfig(ServerConfig &config);

		int			_cgiTmpFilefd;
		int			client_socket;
		int			requested_file;
		std::string	_cgiTmpFileName;
		std::map<std::string, std::string> _req_header;

	private:
		Shared _shared;
		ServerConfig _config;
		ServerLocation _workingLocation;
		/*
			* Member Variables
		*/
		std::string _method;
		std::string _uri; // The Request-URI (Uniform Resource Identifier)
		std::string _path;
		std::string _postFilePath;
		std::string _querystring;
		std::string _chunkHex;
		int			_chunkHexState;
		char		buffer[CHUNK_SIZE];
		int			bytesread;
		int			bytessent;
		int 		_postRecv;
		int 		_chunkSize;
		int 		_postFileFd;
		int 		_headerflag;


		/*
			* Member Functions
		*/
		int 	parseRequestHeader(char *req, int bytesreceived);
		int		HandleGet();
		int		HandleDelete();
		int 	HandlePost(char *body, int bytesreceived);
		int		chunkedPost(char *body, int bytesreceived);
		int		HandleCgi(std::string path, std::string method, int header_flag, cgi &cgitype);
		int		postCgi(cgi &cgiInfo, bool isBoundary);
		char	**CgiSetEnv(std::string method);
		bool	ValidateRequest();
		bool	MatchLocation();
		bool	ValidateURI(const std::string &uri);
		void	printRequstData();
		void	SendResponseHeader(std::string statusCode, std::string fileExt, std::string location, int contentLength);
		void	sendCodeResponse(std::string statusCode);
		void	DeleteFile(const char *path);
		int		DeleteDirectory(const char *path);
		std::string generateListDir(std::string statusCode, std::string ls);
};

#endif