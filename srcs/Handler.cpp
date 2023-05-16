#include "../includes/Handler.hpp"

std::string Handler::GetMimeType()
{
	Shared mime_map;
	int dot_position;
	std::string extention;
	std::map<std::string, std::string>::iterator it;

	// case of path with a file extention
	dot_position = this->_uri.find_last_of(".");
	if (dot_position >= 0)
	{
		extention = this->_uri.substr(dot_position);
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

// Request method getter
std::string Handler::GetRequestMethod()
{
	return (this->_method);
}

// Request path getter
std::string Handler::GetRequestURI()
{
	return (this->_uri);
}

// ------------- METHODS -------------

void Handler::ParseRequestHeader(char *req, ServerConfig &config)
{
	int delimiter_position;
	std::string current_line, key, value;
	char *body;

	std::string request = req;
	size_t header_len = request.find("\r\n\r\n");		// Find the end of the request header
	std::string header = request.substr(0, header_len); // Save the header
	body = req + header_len + 4;						// Save the body
	std::stringstream request_stream(header);

	request_stream >> std::skipws >> std::ws >> this->_method; // Streaming methode into _methode while take care of white spaces
	request_stream >> std::skipws >> std::ws >> this->_uri;   // same for path
	std::getline(request_stream, current_line);				   // skip the remaining part of the request line (HTTP/1.1)

	while (getline(request_stream >> std::ws >> std::skipws, current_line, '\n'))
	{
		current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end()); // remove every occurence of '/r' in line
		delimiter_position = current_line.find(':');
		key = current_line.substr(0, delimiter_position);
		value = current_line.substr(delimiter_position + 2, current_line.length()); // [delimiter_position + 2] to remove the extra space before value
		this->_req_header[key] = value;												// storing key and value in map
	}


	if (!this->validateRequest())
		return;
	
	// TODO vlaidate the request header content
	// Print Header key and values
	// std::cout << "----------------- Request Header ---------------------\n";
	// std::cout << "method: |" << this->_method << "|\n";
	// std::cout << "path: |" << this->_uri << "|\n";
	// for (std::map<std::string, std::string>::const_iterator it = this->_req_header.begin(); it != this->_req_header.end(); ++it)
	// {
	// 	std::cout << it->first << ": |" << it->second << "|\n";
	// }
	if (this->_method == "GET")
		this->HandleGet();
	else if (this->_method == "POST")
		this->HandlePost(body);
	else if (this->_method == "DELETE")
		this->HandleDelete();
}

// Check for Possiple error in the Request
bool	Handler::validateRequest()
{
	// if Transfer-Encoding exist and not match [chunked]
	if (this->_req_header.find("Transfer-Encoding") != this->_req_header.end()
		&& this->_req_header["Transfer-Encoding"] != "chunked")
	{
		// TODO: 501 not implemented
		return false;
	}
	// if both Transfer-Encoding and Content-Length not provided
	if (this->_req_header.find("Transfer-Encoding") == this->_req_header.end()
		&& this->_req_header.find("Content-Length") == this->_req_header.end())
	{
		// TODO: 400 bad request
		return false;
	}
    // URI should start with a leading slash ("/") and not contain any illegal characters
	if (this->validateURI(this->_uri))
	{
		// TODO: 400 Bad requst
		return false;
	}
	//  URI should not have more than 2048
	if (this->_uri.length() > 2048)
	{
		// TODO: 414 requst uri too long
		return false;
	}
	//  Request body size should not be more than [client_body_size] from confing file
	if (this->_uri.length() > 2048)
	{
		// TODO: 414 requst uri too long
		return false;
	}
	return this->matchLocation();
}

// match location from the config file and validate method
bool Handler::matchLocation()
{
	return true;
}

// Check for any any illegal characters in the URI
bool Handler::validateURI(const std::string& uri)
{
    // Check if the URI starts with a leading slash ("/")
    if (uri.empty() || uri[0] != '/')
        return false;

    // Check for any invalid characters in the URI
    const std::string invalidChars = " <>{}|\\^`";
    if (uri.find_first_of(invalidChars) != std::string::npos)
        return false;
    return true;
}

void Handler::HandlePost(char *body)
{
	std::string mimeType, boundry;
	std::string content_type = this->_req_header["Content-Type"];
	std::string encodingFormat = this->_req_header["Transfer-Encoding"];
	mimeType = content_type.substr(0, content_type.find(';'));
	mimeType = content_type.substr(content_type.find(';') + 2, content_type.length());
	if (this->_req_header["Content-Type"] == "application/octet-stream")
	{
		// Create a file stream for writing
		std::ofstream file("/tmp/file", std::ios::out | std::ios::binary);
		if (!file)
		{
			std::cerr << "Error : Failed to open file\n";
        	exit(1);
		}
		else
		{
			// Write the request body data to the file
			file.write(body, std::stoi(this->_req_header["Content-Length"]));

			// Close the file
			file.close();

			// File saved successfully
		}
	}
}

void Handler::HandleGet() {}

void Handler::HandleDelete() {} 