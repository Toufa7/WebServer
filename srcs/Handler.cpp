#include "../includes/Handler.hpp"

//  ------------- CONSTRUCTOR && DESTRUCTOR --------------------


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

void Handler::setConfig(ServerConfig &config) {
	this->_config = config;
}

// ------------- METHODS -------------

void Handler::printRequstData()
{
	// Print Header key and values
	std::cout << "----------------- Request Header ---------------------\n";
	std::cout << "method: |" << this->_method << "|\n";
	std::cout << "path: |" << this->_uri << "|\n";
	for (std::map<std::string, std::string>::const_iterator it = this->_req_header.begin(); it != this->_req_header.end(); ++it)
	{
		std::cout << it->first << ": |" << it->second << "|\n";
	}
}

void Handler::errorResponse(std::string statusCode)
{
    std::stringstream response;
    std::string statusMessage = this->_shared.status_codes[statusCode];
	std::string htmlContent = "<html><head><title>" + statusCode + " " + statusMessage + "</title></head>"
                              "<body><h1>" + statusCode + " " + statusMessage + "</h1></body></html>";
    // TODO: check for this error page in config first
    response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
	response << "Server: " << this->_config.GetServerName() << "\r\n";
    response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << htmlContent.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
	response << htmlContent;

    if (send(this->_config.getClientSocket(), response.str().c_str(), response.str().length(), 0) == -1)
    {   
        std::cerr << "Error : Receiving failed\n";
        exit(1);
    }
	std::cout << "\n---------------------- error response --------------------- " << std::endl;
	std::cout << response.str() << std::endl;
}


void    Handler::fileResponse(std::string path, std::string statusCode)
{
	std::stringstream response;
	std::string statusMessage = this->_shared.status_codes[statusCode];
    struct stat file_infos;
    int         fd;
    char        *response_body;
    int         nbyte;
    if ((fd = open(path.c_str(), O_RDONLY)) == -1)
    {
        std::cerr << "Error : Opening failed\n";
        exit(1);
    }
    if (fstat(fd, &file_infos) == -1)
    {
        std::cerr << "Error : Failed to obtain informations\n";
        exit(1);
    }
    response_body = new char[file_infos.st_size];
    if ((nbyte = read(fd, response_body, file_infos.st_size)) == -1)
    {
        std::cerr << "Error : Reading failed\n";
        exit(1);
    }
	response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
	response << "Server: " << this->_config.GetServerName() << "\r\n";
    response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << file_infos.st_size << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    if (send(this->_config.getClientSocket(), response.str().c_str(), response.str().length(), 0) == -1)
    {   
        std::cerr << "Error : Sending failed\n";
        exit(1);
    }
	if (send(this->_config.getClientSocket(), response_body, nbyte, 0) == -1)
    {   
        std::cerr << "Error : Sending failed\n";
        exit(1);
    }
	std::cout << "\n----------------------file response --------------------- " << std::endl;
	std::cout << response.str() << std::endl;
    delete [] response_body;
}

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

void Handler::ParseRequestHeader(char *req)
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
	request_stream >> std::skipws >> std::ws >> this->_uri;	   // same for path
	std::getline(request_stream, current_line);				   // skip the remaining part of the request line (HTTP/1.1)

	while (getline(request_stream >> std::ws >> std::skipws, current_line, '\n'))
	{
		current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end()); // remove every occurence of '/r' in line
		delimiter_position = current_line.find(':');
		key = current_line.substr(0, delimiter_position);
		value = current_line.substr(delimiter_position + 2, current_line.length()); // [delimiter_position + 2] to remove the extra space before value
		this->_req_header[key] = value;												// storing key and value in map
	}
	// Validate request content
	if (!this->validateRequest())
		return;
	printRequstData();
	if (this->_method == "GET")
		this->HandleGet();
	else if (this->_method == "POST")
		this->HandlePost(body);
	else if (this->_method == "DELETE")
		this->HandleDelete();
}

// Check for Possiple error in the Request
bool Handler::validateRequest()
{
	// if Transfer-Encoding exist and not match [chunked]
	if (this->_req_header.find("Transfer-Encoding") != this->_req_header.end() && this->_req_header["Transfer-Encoding"] != "chunked")
	{
		this->errorResponse("501");
		return false;
	}
	// if both Transfer-Encoding and Content-Length not provided
	if (this->_method == "POST" && this->_req_header.find("Transfer-Encoding") == this->_req_header.end() &&
		this->_req_header.find("Content-Length") == this->_req_header.end())
	{
		this->errorResponse("400");
		return false;
	}
	// URI should start with a leading slash ("/") and not contain any illegal characters
	if (!this->validateURI(this->_uri))
	{
		this->errorResponse("400");
		return false;
	}
	//  URI should not have more than 2048
	if (this->_uri.length() > 2048)
	{
		this->errorResponse("414");
		return false;
	}
	//  Request body size should not be more than [client_body_size] from confing file
	if (this->_req_header.find("Content-Length") != this->_req_header.end() && 
		std::stoll(this->_req_header["Content-Length"]) > std::stoll(this->_config.GetClientBodySize()))
	{
		this->errorResponse("413");
		return false;
	}
	return this->matchLocation();
}

// match location from the config file and validate method
bool Handler::matchLocation()
{
	std::string path = this->_uri;
	std::vector<ServerLocation> serverLocations = this->_config.GetLocationsVec();

	// Seperate Path from args if there is any
	if (this->_uri.find('?') != std::string::npos)
		path = this->_uri.substr(0, this->_uri.find('?'));

	// match path with to the right location
	size_t i;
	for (i = 0; i < serverLocations.size(); i++)
	{
		if (serverLocations[i].GetLocationPath() == path)
		{
			std::vector<std::string> allowedMethods = serverLocations[i].GetAllowedMethodsVec();
			if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_method) == allowedMethods.end())
			{
				this->errorResponse("405");
				return false;
			}
			// Check for location redirection
			break;
		}
	}
	if (i == serverLocations.size())
	{
		this->errorResponse("404");
		return false;
	}
	return true;
}

// Check for any any illegal characters in the URI
bool Handler::validateURI(const std::string &uri)
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
	(void) body;
	std::string encodingFormat, boundary;
	std::string mimeType = "application/octet-stream"; // Default MIME type
	// Save MIME type and boundary if it exist
	if (this->_req_header.find("Content-Type") != this->_req_header.end())
	{
		int semiPos = this->_req_header["Content-Type"].find(';');
		if (semiPos != -1)
		{
			mimeType = this->_req_header["Content-Type"].substr(0, semiPos);
			if (mimeType == "multipart/form-data")
				boundary = this->_req_header["Content-Type"].substr(semiPos + 11, this->_req_header["Content-Type"].length());
		}
		else
			mimeType = this->_req_header["Content-Type"];
	}
	// Save Transfer-Encoding if it exist. 
	if (this->_req_header.find("Transfer-Encoding") != this->_req_header.end())
		encodingFormat = this->_req_header["Transfer-Encoding"];
	if (boundary.empty() && encodingFormat.empty())
	{
		// Create a file stream for writing
		std::string fileName = this->_shared.generateFileName("upload/", this->_shared.file_extensions[mimeType]);
		std::cout << "---------> " << fileName << std::endl;
		std::ofstream file(fileName, std::ios::out | std::ios::binary);
		if (!file)
		{
			std::cerr << "Error : Failed to open file\n";
			exit(1);
		}
		else
		{
			// Write the request body data to the file
			file.write(body, std::stoull(this->_req_header["Content-Length"]) * 100);

			// Close the file
			file.close();

			// File saved successfully
		}
	}
	fileResponse("test/homepage.html", "200");
}

void Handler::HandleGet() {
	fileResponse("test/homepage.html", "200");
}

void Handler::HandleDelete() {
	fileResponse("test/homepage.html", "200");
}