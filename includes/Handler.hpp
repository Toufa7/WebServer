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

		void	parseRequest(char *req);
		void	parseBody(char *body);

	private:
    	std::map<std::string, std::string> _request;

		void	handlePost();
		void	handleGet();
		void	handleDelete();
};

#endif
