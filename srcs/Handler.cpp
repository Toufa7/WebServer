#include "../includes/Handler.hpp"

std::string Handler::GetMimeType()
{
	Shared mime_map;
	int dot_position;
	std::string extention;
	std::map<std::string, std::string>::iterator it;

	// case of path with a file extention
	dot_position = _path.find_last_of(".");
	if (dot_position >= 0)
	{
		extention = _path.substr(dot_position);
		it = mime_map.mime_types.find(extention);
		if (it != mime_map.mime_types.end())
			return (it->second);
	}
	// other cases that should be handled
	return ("text/html");
}

//  ------------- CONSTRUCTOR && DESTRUCTOR --------------------

Handler::Handler() {}

Handler::~Handler() {}

//  ------------- ACCESSOR --------------------

//Request header method getter
std::string Handler::GetHeaderMethod()
{
	return(_method);
}

//Request header path getter
std::string Handler::GetHeaderPath()
{
	return(_path);
}

// ------------- METHODS ------------- 
void Handler::ParseRequestHeader(char *req)
{
	int delimiter_position;
	std::string current_line, key, value, request_line;
	std::stringstream request_stream(req);

	std::getline(request_stream, request_line);						// Request-line
	std::stringstream request_line_stream(request_line);
	request_line_stream >> std::skipws >> std::ws >> this->_method;	// Streaming methode into _methode while take care of white spaces
	request_line_stream >> std::skipws >> std::ws >> this->_path;	// same for path

	while (getline(request_stream >> std::ws >> std::skipws, current_line, '\n'))
	{
		current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end()); // remove every occurence of '/r' in line
		delimiter_position = current_line.find(':');
		key = current_line.substr(0, delimiter_position);
		value = current_line.substr(delimiter_position + 2, current_line.length()); // [delimiter_position + 2] to remove the extra space before value
		this->_request[key] = value;												// storing key and value in map
		key.erase();																// erase key and value
		value.erase();																// for next iteration
	}

	// // Print key and values
	std::cout << "----------------- Request Header ---------------------\n";
	for (std::map<std::string, std::string>::const_iterator it = this->_request.begin(); it != this->_request.end(); ++it)
	{
		std::cout << it->first << ": |" << it->second << "|\n";
	}
	if (this->_method == "GET")
	{
		// std::cout << "		GET REQUEST\n";
		this->HandleGet();
	}
	else if (this->_method == "POST")
	{
		// std::cout << "		POST REQUEST\n";
		this->HandlePost();
	}
	else if (this->_method == "DELETE")
	{
		// std::cout << "		DELETE REQUEST\n";
		this->HandleDelete();
	}
	else
	{
		// TODO: method not found response
	}
	GetMimeType();
}

void Handler::ParseBody(char *body)
{
	(void)body;
}

void Handler::HandlePost()
{
}

void Handler::HandleGet() {}

void Handler::HandleDelete() {}