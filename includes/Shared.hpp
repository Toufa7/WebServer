#ifndef SHARED_HPP
#define SHARED_HPP

#define FALSE 0
#define TRUE 1


#include <iostream>
#include <map>
#include <cstring>
#include <sstream>
#include <vector>
#include <fstream>

class Shared
{
	public:
		Shared();

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
