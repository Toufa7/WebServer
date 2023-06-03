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

void Handler::setConfig(ServerConfig &config)
{
	this->_config = config;
}

int	Handler::WorkingLocationIndex(void)
{
	return _WorkingLocationIndex;
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

std::string Handler::generatePageCode(std::string statusCode)
{
	std::ifstream file("smaples/errorPageSample.html");
	std::string statusMessage = this->_shared.status_codes[statusCode];
	std::string res = "<html><head><title>" + statusCode + " " + statusMessage + "</title></head>"
	"<body><h1>" + statusCode + " " + statusMessage + "</h1></body></html>";
	std::stringstream buffer;
	if (file)
	{
		buffer << file.rdbuf();
		file.close();
		res = buffer.str();
		res.insert(res.find("<title><") + 7, statusCode + " - " + statusMessage);
		res.insert(res.find("<h1><") + 4, statusCode + " - " + statusMessage);
	}
	return res;
}

std::string Handler::generateListDir(std::string statusCode, std::string ls)
{
	//5asek tcheki wach ls 3amra wla 5awya
	std::stringstream s(ls);
	std::string statusMessage = this->_shared.status_codes[statusCode];
	std::string TmpStr, res = "<html><head><title>Directory list</title></head><body><h1><ul>";
	while (std::getline(s, TmpStr,'\n'))
	{
		if ( TmpStr != ".." )
		{
			res += "<li><a href=\"";
			res += TmpStr;
			res += "\">";
			res += TmpStr.substr(TmpStr.find_last_of("/") + 1, TmpStr.length());
			res += "</a></li>";
		}
	}
	res += "</h1></body></html>";
	return res;
}

void Handler::codeResponse(std::string statusCode)
{
	std::stringstream response;
	std::string htmlContent;

	// Check for this error page in the server config
	if (this->_config._ErrorPageMap.find(statusCode) != this->_config._ErrorPageMap.end())
	{
		std::ifstream file(this->_config._ErrorPageMap[statusCode].c_str());
		std::stringstream buffer;
		if (file)
		{
			buffer << file.rdbuf();
			htmlContent = buffer.str();
			file.close();
		}
		else
			htmlContent = this->generatePageCode(statusCode);
	}
	else
		htmlContent = this->generatePageCode(statusCode);

	response << "HTTP/1.1 " << statusCode << " " << this->_shared.status_codes[statusCode] << "\r\n";
	response << "Server: " << this->_config.GetServerNames() << "\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << htmlContent.length() << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << htmlContent;

	if (send(this->_config.getClientSocket(), response.str().c_str(), response.str().length(), 0) == -1)
	{
		std::cerr << "Error : Sending failed\n";
		exit(1);
	}
	std::cout << "\n---------------------- error response --------------------- " << std::endl;
	std::cout << response.str() << std::endl;
}

void Handler::getcodeResponse(std::string statusCode, std::string htmlContent)
{
	std::stringstream response;

	response << "HTTP/1.1 " << statusCode << " " << this->_shared.status_codes[statusCode] << "\r\n";
	response << "Server: " << this->_config.GetServerNames() << "\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << htmlContent.length() << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << htmlContent;

	if (send(this->_config.getClientSocket(), response.str().c_str(), response.str().length(), 0) == -1)
	{
		std::cerr << "Error : Sending failed\n";
		exit(1);
	}
	// std::cout << "\n---------------------- [getcodeResponse()] error response --------------------- " << std::endl;
	// std::cout << response.str() << std::endl;
}

void	Handler::redirectionResponse(std::string statusCode, std::string location)
{
	std::stringstream response;

	response << "HTTP/1.1 " << statusCode << " " << this->_shared.status_codes[statusCode] << "\r\n";
	response << "Location: " << location << "\r\n";
	response << "Server: " << this->_config.GetServerNames() << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";

	if (send(this->_config.getClientSocket(), response.str().c_str(), response.str().length(), 0) == -1)
	{
		std::cerr << "Error : Sending failed\n";
		exit(1);
	}
	std::cout << "\n---------------------- redirection response --------------------- " << std::endl;
	std::cout << response.str() << std::endl;
}

void Handler::fileResponse(std::string path, std::string statusCode)
{
	std::stringstream response;
	std::stringstream body;
	std::ifstream file(path);

	if (!file)
	{
		codeResponse("500");
		return;
	}
	body << file.rdbuf();
	response << "HTTP/1.1 " << statusCode << " " << this->_shared.status_codes[statusCode] << "\r\n";
	response << "Server: " << this->_config.GetServerNames() << "\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << body.str().length() << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << body.str();
	if (send(this->_config.getClientSocket(), response.str().c_str(), response.str().length(), 0) == -1)
	{
		std::cerr << "Error : Sending failed\n";
		exit(1);
	}
	std::cout << "\n---------------------- file response --------------------- " << std::endl;
	std::cout << response.str() << std::endl;
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

	request_stream >> std::skipws >> std::ws >> this->_method; // Streaming methode into _methode while taking care of white spaces
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
	printRequstData();
	// Validate request content
	if (!this->validateRequest())
		return;
	
	if (this->_method == "GET")
		this->HandleGet();
//	else if (this->_method == "POST")
//		this->HandlePost(body);
//	else if (this->_method == "DELETE")
//		this->HandleDelete();
}

// Check for Possible error in the Request
bool Handler::validateRequest()
{
	// if Transfer-Encoding exist and not match [chunked]
	if (this->_req_header.find("Transfer-Encoding") != this->_req_header.end() && this->_req_header["Transfer-Encoding"] != "chunked")
	{
		this->codeResponse("501");
		return false;
	}
	// if both Transfer-Encoding and Content-Length not provided
	if (this->_method == "POST" && this->_req_header.find("Transfer-Encoding") == this->_req_header.end() &&
		this->_req_header.find("Content-Length") == this->_req_header.end())
	{
		this->codeResponse("400");
		return false;
	}
	// URI should start with a leading slash ("/") and not contain any illegal characters
	if (!this->validateURI(this->_uri))
	{
		this->codeResponse("400");
		return false;
	}
	//  URI should not have more than 2048
	if (this->_uri.length() > 2048)
	{
		this->codeResponse("414");
		return false;
	}
	//  Request body size should not be more than [client_body_size] from confing file
	if (this->_req_header.find("Content-Length") != this->_req_header.end() &&
		std::stoll(this->_req_header["Content-Length"]) > std::stoll(this->_config.GetClientBodySize()))
	{
		this->codeResponse("413");
		return false;
	}
	return this->matchLocation();
}

// match location from the config file and validate method
bool Handler::matchLocation()
{
	std::string path = this->_uri;
	std::vector<ServerLocation> serverLocations = this->_config.GetLocationsVec();
	int tmp_index = 0;
	size_t i = 0, old_size = 0, root_location = 0;

	// Seperate Path from args if there is any
	if (this->_uri.find('?') != std::string::npos)
		path = this->_uri.substr(0, this->_uri.find('?'));

	//find the closest location to requested resource
	for (i = 0; i < serverLocations.size(); i++)
	{
		tmp_index = path.find(serverLocations[i].GetLocationPath());
		if (tmp_index != -1)
		{
			if (serverLocations[i].GetLocationPath() == "/")//saving '/' location
				root_location = i; 
			if ((path[0] == '/') && (path.length() == 1))// case of '/' only
				_WorkingLocationIndex = i;
			if (serverLocations[i].GetLocationPath().length() > old_size)//case of closest valid location
			{
				old_size = serverLocations[i].GetLocationPath().length();
                _WorkingLocationIndex = i;
			}
			if ((path[0] == '/') && (i == serverLocations.size()) && old_size == 0)//case of '/not_valid'
                _WorkingLocationIndex = root_location;
		}
	}


	if (serverLocations[_WorkingLocationIndex].GetRedirectionInfo().RedirectionFlag)
	{
		std::string path = serverLocations[_WorkingLocationIndex].GetRedirectionInfo().RedirectionPath;
		this->redirectionResponse(serverLocations[_WorkingLocationIndex].GetRedirectionInfo().RedirectionCode, path);
		return false;
	}
	// std::cout << "I Enter\n";
	// exit(0);
	std::vector<std::string> allowedMethods = serverLocations[_WorkingLocationIndex].GetAllowedMethodsVec();
	if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_method) == allowedMethods.end())
	{
		this->codeResponse("405");
		return false;
	}
	// Check for location redirection

	//based on new design, path will be checked ferther in every method for validity
	
	// if (i == serverLocations.size())
	// {
	// 	this->codeResponse("404");
	// 	return false;
	// }

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
	(void)body;
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

void Handler::HandleGet()
{
	int			RepStrPos;
    size_t      i = 0;
    std::string	RealPath, tmp_str, DirStr, UriInit;
	struct stat	s, t;

	//std::cout << "_uri get() -->" << _uri << "<--\n";
	UriInit = _uri;
	RepStrPos = _uri.find_first_of(this->_config.GetLocationsVec()[_WorkingLocationIndex].GetLocationPath());
    if (RepStrPos < 0)
	{
		std::cout << "I entred here\n";
        this->codeResponse("404");
	}
    else
        _uri.replace(0, this->_config.GetLocationsVec()[_WorkingLocationIndex].GetLocationPath().length(), this->_config.GetLocationsVec()[_WorkingLocationIndex].GetRoot());
	//std::cout << "_uri get() 000>" << _uri << "<000\n";
	
	if (stat(_uri.c_str(),&s) == 0)
	{
		/*------------------------------------------- DIR Handler ----------------------------------------------------*/
    	if ( s.st_mode & S_IFDIR )
    	{
			if (_uri[_uri.size() - 1] != '/')
                _uri += '/';
            if (this->_config.GetLocationsVec()[_WorkingLocationIndex].GetIndexesVec().empty() == 0)//identify the working index
			{
				for (i = 0; i < this->_config.GetLocationsVec()[_WorkingLocationIndex].GetIndexesVec().size(); i++)
				{
					tmp_str = _uri;
					tmp_str += this->_config.GetLocationsVec()[_WorkingLocationIndex].GetIndexesVec()[i];
					if (stat(tmp_str.c_str(), &t) == 0)
						break;
					tmp_str.erase();
				}
			}
			if (tmp_str.empty() == 0)
			{
				//case of valid index file so you shuold handle cgi or not
			}
			else
			{
				//case of no index file and should check auto index:handled
                if (this->_config.GetLocationsVec()[_WorkingLocationIndex].GetAutoIndex() == 1)
                {
                    DIR *DirPtr;
                    struct dirent *Dir;
                    DirPtr = opendir(_uri.c_str());
                    if (DirPtr)
                    {
						std::string IfDir = UriInit;
						if (IfDir[IfDir.length() - 1] != '/')
							IfDir += '/';
                        while ((Dir = readdir (DirPtr)) != NULL)
						{
							DirStr += IfDir;
							DirStr += Dir->d_name;
							DirStr += '\n';
						}
                        closedir(DirPtr);
                    }
                    if (DirStr.empty() == 0)
                        this->getcodeResponse("200", generateListDir("200", DirStr));
						//addi hna l function li radi t handli l auto indexing
                }
                else
                    this->codeResponse("403");//case of no index file and no index file:handled
            }
    	}
		/*------------------------------------------- DIR Handler end ------------------------------------------------*/

		/*--------------------------------------------- File Handler -------------------------------------------------*/
    	else if( s.st_mode & S_IFREG )
    	{
			if (this->_config.GetLocationsVec()[_WorkingLocationIndex].GetCgiInfo().path != "n/a")
			{
				//handle file cgi
			}
			else
			{
				//part flag
				std::cout << "File size is :" << s.st_size << "\n";
				GetFileResponse("200", this->_uri);
			}
    	}
		/*--------------------------------------------- File Handler -------------------------------------------------*/
	}
	else
        this->codeResponse("404");
	//fileResponse("test/homepage.html", "200");
}

void Handler::GetFileResponse(std::string statusCode, std::string path)
{
	std::stringstream response;
	std::string FileExtention, htmlContent;

	FileExtention = path.substr(path.find_last_of('.'), path.length());
	std::ifstream file(path.c_str());
	std::stringstream buffer;
	if (file)
	{
		buffer << file.rdbuf();
		htmlContent = buffer.str();
		file.close();
	}
	response << "HTTP/1.1 " << statusCode << " " << this->_shared.status_codes[statusCode] << "\r\n";
	response << "Server: " << this->_config.GetServerNames() << "\r\n";
	response << "Content-Type: " << this->_shared.mime_types[FileExtention] << "\r\n";
	response << "Content-Length: " << htmlContent.length() << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << htmlContent;

	if (send(this->_config.getClientSocket(), response.str().c_str(), response.str().length(), 0) == -1)
	{
		std::cerr << "Error : Sending failed\n";
		exit(1);
	}
}

void Handler::HandleDelete()
{
	fileResponse("test/homepage.html", "200");
}