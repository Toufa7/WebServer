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
	// other cases should be handled
	return ("text/html");
}

//  ------------- CONSTRUCTOR && DESTRUCTOR --------------------

Handler::Handler() {}

Handler::~Handler() {}

//  ------------- ACCESSOR --------------------

// Request method getter
std::string Handler::GetRequestMethod()
{
	return (_method);
}

// Request path getter
std::string Handler::GetRequestPath()
{
	return (_path);
}

// ------------- METHODS -------------
void Handler::ParseRequestHeader(char *req)
{
	int delimiter_position;
	std::string current_line, key, value;

	std::string request = req;
	size_t header_len = request.find("\r\n\r\n");		// Find the end of the request header
	std::string header = request.substr(0, header_len); // Save the header
	std::string body = request.substr(header_len + 4);	// Save the body
	std::stringstream request_stream(header);

	request_stream >> std::skipws >> std::ws >> this->_method; // Streaming methode into _methode while take care of white spaces
	request_stream >> std::skipws >> std::ws >> this->_path;   // same for path
	std::getline(request_stream, current_line);				   // skip the remaining part of the request line (HTTP/1.1)

	while (getline(request_stream >> std::ws >> std::skipws, current_line, '\n'))
	{
		current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end()); // remove every occurence of '/r' in line
		delimiter_position = current_line.find(':');
		key = current_line.substr(0, delimiter_position);
		value = current_line.substr(delimiter_position + 2, current_line.length()); // [delimiter_position + 2] to remove the extra space before value
		this->_req_header[key] = value;												// storing key and value in map
	}

	// TODO vlaidate the request header content
	// Print Header key and values
	std::cout << "----------------- Request Header ---------------------\n";
	std::cout << "method: |" << this->_method << "|\n";
	std::cout << "path: |" << this->_path << "|\n";
	for (std::map<std::string, std::string>::const_iterator it = this->_req_header.begin(); it != this->_req_header.end(); ++it)
	{
		std::cout << it->first << ": |" << it->second << "|\n";
	}
	if (this->_method == "GET")
		this->HandleGet();
	else if (this->_method == "POST")
		this->HandlePost();
	else if (this->_method == "DELETE")
		this->HandleDelete();
	else
	{
		// TODO: method not found response
	}
	GetMimeType();
}

void Handler::HandlePost()
{
}

void Handler::HandleGet() {}

void Handler::HandleDelete() {}