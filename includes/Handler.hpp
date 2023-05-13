#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <map>
#include <cstring>
#include <sstream>
#include "Shared.hpp"

class Handler
{
	public:
		Handler();
		~Handler();

		void	ParseRequestHeader(char *req);
		void	ParseBody(char *body);

	private:
    	std::map<std::string, std::string> _request;
		std::string _method;
		std::string _path;

		void	HandlePost();
		void	HandleGet();
		void	HandleDelete();
		std::string GetHeaderMethod();
		std::string GetHeaderPath();
		std::string GetMimeType();


};

#endif
