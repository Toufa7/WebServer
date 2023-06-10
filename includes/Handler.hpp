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
		void		ParseRequestHeader(char *req);

 		std::string GetRequestMethod();
		std::string GetRequestURI();
		std::string GetMimeType();
		std::string GetStatusCode();
		void 		setConfig(ServerConfig &config);
		int			WorkingLocationIndex(void);

	private:
    	std::map<std::string, std::string> _req_header;
		std::string _method;
		std::string _uri; // The Request-URI (Uniform Resource Identifier) 
		std::string _status_code;
		std::string _status_text;
		Shared 		_shared;
		ServerConfig _config;
		int			_WorkingLocationIndex;

		void	HandlePost(char *body);
		void	HandleGet();
		void	HandleDelete();
		void	DeleteFile(const char *path);
		void	DeleteDirectory(const char *path);
		bool	validateRequest();
		bool 	matchLocation();
		bool 	validateURI(const std::string& uri);
		void	printRequstData();
		void    fileResponse(std::string path, std::string statusCode);
		void 	codeResponse(std::string statusCode);
		void	getcodeResponse(std::string statusCode, std::string htmlContent);
		void	redirectionResponse(std::string statusCode, std::string location);
		std::string generatePageCode(std::string statusCode);
		std::string generateListDir(std::string statusCode, std::string ls);
		void	GetFileResponse(std::string statusCode, std::string path);

};

#endif
