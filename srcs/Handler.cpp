#include "../includes/Handler.hpp"

Handler::Handler() {}

Handler::~Handler() {}

void Handler::parseRequest(char *req)
{
	int delimiter_position;
	std::string current_line, key, value, status_line;
	std::stringstream request_stream(req);

	std::getline(request_stream, status_line); // status line
	while (getline(request_stream >> std::ws >> std::skipws, current_line, '\n'))
	{
		current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end()); // remove every occurence of '/r' in line
		delimiter_position = current_line.find(':');
		key = current_line.substr(0, delimiter_position);
		value = current_line.substr(delimiter_position + 1, current_line.length());
		this->_request[key] = value; // storing key and value in map
		key.erase();				 // erase key and value
		value.erase();				 // for next iteration
	}
}

void Handler::parseBody(char *body) {
	(void) body;
}

void Handler::handlePost() {}

void Handler::handleGet() {}

void Handler::handleDelete() {}