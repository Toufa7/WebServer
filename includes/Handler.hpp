#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <map>
#include <cstring>
#include <sstream>

class Handler
{
	public:
		Handler();
		~Handler();

		void	ParseRequest(char *req);
		void	ParseBody(char *body);

	private:
    	std::map<std::string, std::string> _request;

		void	HandlePost();
		void	HandleGet();
		void	HandleDelete();
};

#endif
