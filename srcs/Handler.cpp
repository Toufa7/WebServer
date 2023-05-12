#include "../includes/Handler.hpp"

Handler::Handler() {}

Handler::~Handler() {}

void Handler::ParseRequestHeader(char *req)
{
	int delimiter_position;
	std::string current_line, key, value, request_line;


	std::stringstream request_stream(req);

	std::getline(request_stream, request_line); // Request-line
	std::cout << "---->" << request_line << std::endl;
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
	std::cout << "-----------------Response Message ---------------------\n";
	for (std::map<std::string, std::string>::const_iterator it = this->_request.begin(); it != this->_request.end(); ++it)
	{
		std::cout << "Key =>	" << it->first << "	Value => " << it->second << "|\n";
	}
	if (request_line.substr(0, 4) == "GET ")
	{
		// std::cout << "		GET REQUEST\n";
		this->HandleGet();
	}
	else if (request_line.substr(0, 5) == "POST ")
	{
		// std::cout << "		POST REQUEST\n";
		this->HandlePost();
	}
	else if (request_line.substr(0, 6) == "DELETE ")
	{
		// std::cout << "		DELETE REQUEST\n";
		this->HandleDelete();
	}
	else
	{
		// TODO: method not found response
	}
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