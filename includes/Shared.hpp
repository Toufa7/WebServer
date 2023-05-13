#ifndef SHARED_HPP
#define SHARED_HPP

#include <iostream>
#include <map>
#include <cstring>
#include <sstream>

class Shared
{
	public:
		Shared();
		//~Shared();

    	std::map<std::string, std::string> mime_types;
    	std::map<std::string, std::string> status_codes;
};

class Exceptions : public std::exception
{
	std::string _error_message;
	public:
		Exceptions(std::string error_message);
		const char *what() const throw();
		~Exceptions() throw();
};

#endif
