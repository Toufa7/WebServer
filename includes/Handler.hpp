#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <map>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <fstream> 
#include <ios>
#include <regex>
#include "Shared.hpp"
#include "Config.hpp"

class Handler
{
	public:
		Handler();
		~Handler();

		void		ParseRequestHeader(char *req, ServerConfig &config);

 		std::string GetRequestMethod();
		std::string GetRequestURI();
		std::string GetMimeType();
		std::string GetStatusCode();

	private:
    	std::map<std::string, std::string> _req_header;
		std::string _method;
		std::string _uri; // The Request-URI (Uniform Resource Identifier) 
		std::string _status_code;
		std::string _status_text;

		void	HandlePost(char *body);
		void	HandleGet();
		void	HandleDelete();
		bool	validateRequest(ServerConfig &config);
		bool 	matchLocation(ServerConfig &config);
		bool 	validateURI(const std::string& uri);


};

#endif
