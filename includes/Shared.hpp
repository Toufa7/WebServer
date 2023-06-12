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


		std::string generateFileName(const std::string& path, const std::string& fileExtension);

    	std::map<std::string, std::string> mime_types;
    	std::map<std::string, std::string> file_extensions;
    	std::map<std::string, std::string> status_codes;
		std::string fileExtention(std::string fileName);
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
