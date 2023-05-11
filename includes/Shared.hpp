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
		~Shared();

    	std::map<std::string, std::string> mime_types;
    	std::map<std::string, std::string> status_codes;
};

#endif
