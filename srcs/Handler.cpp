#include "../includes/Handler.hpp"

//  ------------- CONSTRUCTOR && DESTRUCTOR --------------------

// TODO add drivder function




//  ------------- ACCESSOR --------------------

std::string Handler::GetRootLocation(std::string uri, std::string locationPath, std::string root)
{
	std::cout << "GetRootLocation been called\n";
	std::cout << "URI is -> " << uri << std::endl;
	std::string MatchedUri;
	if (uri.find(locationPath) != std::string::npos)
	{
		root += '/';
		MatchedUri = uri.replace(0, locationPath.length(), root);
	}
	else
		return (uri);
	return (MatchedUri);
}


void	Handler::Driver(char *requested_data)
{
	if (this->headerflag == 0)
		this->parseRequestHeader(requested_data);
	else
	{
		if (this->_method == "GET")
			this->HandleGet();
		else if (this->_method == "POST")
			this->HandlePost(requested_data);
		else if (this->_method == "DELETE")
			this->HandleDelete();
	}
	this->headerflag++;
}

// Request method getter
std::string Handler::getRequestMethod()
{
	return (this->_method);
}

// Request path getter
std::string Handler::getRequestURI()
{
	return (this->_uri);
}

void Handler::setConfig(ServerConfig &config)
{
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

// Generate a response header from the recieved argements
void Handler::sendResponseHeader(std::string statusCode, std::string fileExt, std::string location, int contentLength)
{
	std::stringstream header;

	header << "HTTP/1.1 " << statusCode << " " << this->_shared.status_codes[statusCode] << "\r\n";
	header << "Server: " << this->_config.GetServerNames() << "\r\n";

	if (!location.empty())
		header << "Location: " << location << "\r\n";
	if (!fileExt.empty())
		header << "Content-Type: " << this->_shared.mime_types[fileExt] << "\r\n";
	if (contentLength)
		header << "Content-Length: " << contentLength << "\r\n";

	header << "Connection: close\r\n";
	header << "\r\n";

	if (send(this->client_socket, header.str().c_str(), header.str().length(), 0) == -1)
		perror("Error : Sending failed -> ");
}

std::string Handler::generateListDir(std::string statusCode, std::string ls) 
{
	// 5asek tcheki wach ls 3amra wla 5awya
	std::stringstream s(ls);
	std::string statusMessage = this->_shared.status_codes[statusCode];
	std::string TmpStr, res = "<html><head><title>Directory list</title></head><body><h1><ul>";
	while (std::getline(s, TmpStr, '\n'))
	{
		if (TmpStr != "..")
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

void Handler::sendErrorResponse(std::string statusCode)
{
	std::string htmlContent;

	// Check for this error page in the server config
	if (this->_config._ErrorPageMap.find(statusCode) != this->_config._ErrorPageMap.end())
	{
		std::ifstream file(this->_config._ErrorPageMap[statusCode].c_str());
		if (file)
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			htmlContent = buffer.str();
			file.close();
		}
	}
	if (htmlContent.empty())
	{
		std::ifstream file("samples/errorPageSample.html");
		std::string statusMessage = this->_shared.status_codes[statusCode];
		std::string res = "<html><head><title>" + statusCode + " " + statusMessage + "</title></head>" + "<body><h1>" +
						  statusCode + " " + statusMessage + "</h1></body></html>";
		if (file)
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			res = buffer.str();
			res.insert(res.find("<title><") + 7, statusCode + " - " + statusMessage);
			res.insert(res.find("<h1><") + 4, statusCode + " - " + statusMessage);
		}
		htmlContent = res;
	}
	sendResponseHeader(statusCode, ".html", "", htmlContent.length());
	if (send(this->client_socket, htmlContent.c_str(), htmlContent.length(), 0) == -1)
		perror("Error : Sending failed -> ");
}

std::string Handler::getMimeType()
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

// -------------------------------- Request parse and validation ------------

void Handler::parseRequestHeader(char *req)
{
		std::cout << "-> " << req << std::endl;
	// 	exit(0);
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
	// printRequstData();
	// Validate request content
	if (!this->validateRequest())
		return;

	if (this->_method == "GET")
		this->HandleGet();
	else if (this->_method == "POST")
		this->HandlePost(body);
	else if (this->_method == "DELETE")
		this->HandleDelete();

}

// Check for Possible error in the Request
bool Handler::validateRequest()
{
	// if Transfer-Encoding exist and not match [chunked]
	if (this->_req_header.find("Transfer-Encoding") != this->_req_header.end() && this->_req_header["Transfer-Encoding"] != "chunked")
	{
		this->sendErrorResponse("501");
		return false;
	}
	// if both Transfer-Encoding and Content-Length not provided
	if (this->_method == "POST" && this->_req_header.find("Transfer-Encoding") == this->_req_header.end() &&
		this->_req_header.find("Content-Length") == this->_req_header.end())
	{
		this->sendErrorResponse("400");
		return false;
	}
	// URI should start with a leading slash ("/") and not contain any illegal characters
	if (!this->validateURI(this->_uri))
	{
		this->sendErrorResponse("400");
		return false;
	}
	//  URI should not have more than 2048
	if (this->_uri.length() > 2048)
	{
		this->sendErrorResponse("414");
		return false;
	}
	//  Request body size should not be more than [client_body_size] from confing file
	if (this->_req_header.find("Content-Length") != this->_req_header.end() &&
		std::stoll(this->_req_header["Content-Length"]) > std::stoll(this->_config.GetClientBodySize()))
	{
		this->sendErrorResponse("413");
		return false;
	}
	return this->matchLocation();
}

// match location from the config file and validate method
bool Handler::matchLocation()
{
	this->_path = this->_uri;
	std::vector<ServerLocation> serverLocations = this->_config.GetLocationsVec();
	size_t i = 0, old_size = 0, root_location = 0;

	// Seperate Path from args if there is any
	if (this->_uri.find('?') != std::string::npos)
		_path = this->_uri.substr(0, this->_uri.find('?'));

	// find the closest location to requested resource
	for (i = 0; i < serverLocations.size(); i++)
	{
		size_t tmp_index = _path.find(serverLocations[i].GetLocationPath());
		if (tmp_index != std::string::npos)
		{
			if (serverLocations[i].GetLocationPath() == "/") // saving '/' location
				root_location = i;
			if ((_path[0] == '/') && (_path.length() == 1)) // case of '/' only
				this->_workingLocation = serverLocations[i];
			if (serverLocations[i].GetLocationPath().length() > old_size) // case of closest valid location
			{
				old_size = serverLocations[i].GetLocationPath().length();
				this->_workingLocation = serverLocations[i];
			}
			if ((_path[0] == '/') && (i == serverLocations.size()) && old_size == 0) // case of '/not_valid'
				this->_workingLocation = serverLocations[root_location];
		}
	}

	// Check for location redirection
	if (this->_workingLocation.GetRedirectionInfo().RedirectionFlag)
	{
		_path = this->_workingLocation.GetRedirectionInfo().RedirectionPath;
		this->sendResponseHeader(this->_workingLocation.GetRedirectionInfo().RedirectionCode, "", _path, 0);
		return false;
	}

	std::vector<std::string> allowedMethods = this->_workingLocation.GetAllowedMethodsVec();
	if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_method) == allowedMethods.end())
	{
		this->sendErrorResponse("405");
		return false;
	}

	//std::cout << "uri is : " << this->_uri << '\n';
	_path = GetRootLocation(this->_uri, this->_workingLocation.GetLocationPath(), this->_workingLocation.GetRoot());
	//_path += '/';
	//std::cout << "Path is : " << _path << '\n';
	// If location not found send a "not found" response
	// if (i == serverLocations.size())
	// {
	// 	this->sendErrorResponse("404");
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

// -------------------------------- POST method ----------------------

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
	this->sendFileResponse("200", "test/homepage.html");
}

// -------------------------------- GET method ----------------------

// Send a respone with [statusCode] in the header and content of the file in [path] as body
void Handler::sendFileResponse(std::string statusCode, std::string path)
{
	(void)statusCode;
	std::string fileExt, content;

	fileExt = path.substr(path.find_last_of('.'), path.length());
	std::ifstream file(path.c_str());
	std::stringstream buffer;
	if (file)
	{
		buffer << file.rdbuf();
		content = buffer.str();
		file.close();
	}
	else
	{
		this->sendErrorResponse("500");
		return;
	}

	// sendResponseHeader(statusCode, fileExt, "", content.length());

	if (send(this->client_socket, content.c_str(), content.length(), 0) == -1)
	{
		perror("Error : Sending failed");
	}
}

void Handler::HandleGet()
{
	static int a = 0;

	// std::cout << "Header flag is " << this->headerflag << '\n';
	std::cout << "\n" << a << " = " << _path << "\n";

	// TODO: Function to match the location, takes locationsVec, and uri , and should return the closest match
	//int RepStrPos;
	size_t i = 0;
	std::string RealPath, tmp_str, DirStr, UriInit;
	struct stat s, t;

	UriInit = _uri;

	// RepStrPos = _uri.find_first_of(this->_workingLocation.GetLocationPath());
	// if (RepStrPos < 0)
	// {
	// 	std::cout << "I entred here\n";
	// 	this->sendErrorResponse("404");
	// }
	// else
	// 	_uri = GetRootLocation(_uri, this->_workingLocation.GetLocationPath(), this->_workingLocation.GetRoot());
	//std::cout << "GetRootLocation been called\n";
	//matchstd::cout << _uri << "\n";
	//exit(0);
	//_uri.replace(0, this->_workingLocation.GetLocationPath().length(), this->_workingLocation.GetRoot());


	if (stat(_path.c_str(), &s) == 0)
	{
		std::cout << "--- here 1 ----\n";
		/*------------------------------------------- DIR Handler ----------------------------------------------------*/
		if (s.st_mode & S_IFDIR)
		{
			if (_path[_path.size() - 1] != '/')
				_path += '/';
			if (this->_workingLocation.GetIndexesVec().empty() == 0) // identify the working index
			{
				for (i = 0; i < this->_workingLocation.GetIndexesVec().size(); i++)
				{
					tmp_str = _path;
					tmp_str += this->_workingLocation.GetIndexesVec()[i];
					if (stat(tmp_str.c_str(), &t) == 0)
						break;
					tmp_str.erase();
				}
			}
			if (tmp_str.empty() == 0)
			{
				// case of valid index file so you shuold handle cgi or not
			}
			else
			{
				// case of no index file and should check auto index:handled
				if (this->_workingLocation.GetAutoIndex() == 1)
				{
					DIR *DirPtr;
					struct dirent *Dir;
					DirPtr = opendir(_path.c_str());
					if (DirPtr)
					{
						std::string IfDir = UriInit;
						if (IfDir[IfDir.length() - 1] != '/')
							IfDir += '/';
						while ((Dir = readdir(DirPtr)) != NULL)
						{
							DirStr += IfDir;
							DirStr += Dir->d_name;
							DirStr += '\n';
						}
						closedir(DirPtr);
					}
					if (DirStr.empty() == 0)
					{
						std::string lsDir = generateListDir("200", DirStr);
						if (this->headerflag == 0)
							sendResponseHeader("200", ".html", "", lsDir.length());
						if (send(this->client_socket, lsDir.c_str(), lsDir.length(), 0) == -1)
							perror("Error : Sending failed");
					}
					// addi hna l function li radi t handli l auto indexing
				}
				else
					this->sendErrorResponse("403"); // case of no index file and no index file:handled
			}
		}
		/*------------------------------------------- DIR Handler end ------------------------------------------------*/

		/*--------------------------------------------- File Handler -------------------------------------------------*/
		else if (s.st_mode & S_IFREG)
		{
			std::cout << "--- here 2 ----\n";
			if (this->_workingLocation.GetCgiInfo().path != "n/a")
			{
				// std::cout << " HAHSKJAHSKJHAKJSHKAJH\n";
				// handle file cgi
			}
			else
			{
				std::cout << "--- here 3 ----\n";
				struct stat file;
				if (this->headerflag == 0)
				{
					std::string filext = _path.substr(_path.find_last_of('.'), _path.length());
					requested_file = open(this->_path.c_str(), O_RDONLY);
					stat(this->_path.c_str(), &file);
					sendResponseHeader("200", filext, "", file.st_size);
				}
				bytesread = read(requested_file, buffer, sizeof(buffer));
				if (bytesread == -1)
					perror("Error (Read) -> ");
				bytessent = send(this->client_socket, buffer, bytesread, 0);
				if (bytessent == -1)
				{
					// DropClient();
					perror("Error (Send) -> ");
				}
				if (bytessent == 0 || bytesread == 0)
				{
					perror("Error (Send) -> ");
					// DropClient();
				}
			}
		}
		/*--------------------------------------------- File Handler -------------------------------------------------*/
	}
	else
	{

		this->sendErrorResponse("404");
	}
	a++;
}

// -------------------------------- Delete method ----------------------

void Handler::DeleteDirectory(const char *path)
{
	DIR *directory;
	struct dirent *dir;
	struct stat file;
	char subdir[256];

	// open a directory
	if ((directory = opendir(path)) == NULL)
	{
		std::cerr << "Cannot Open Directory: " << path << std::endl;
		return;
	}
	// read the contents of the directory
	while ((dir = readdir(directory)) != NULL)
	{
		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			continue;
		snprintf(subdir, sizeof(subdir), "%s/%s", path, dir->d_name);
		if (stat(subdir, &file) == 0)
		{
			if (S_ISREG(file.st_mode))
			{
				std::cout << "File -> " << subdir << std::endl;
				DeleteFile(subdir);
			}
			else if (S_ISDIR(file.st_mode))
			{
				std::cout << "Dir -> " << subdir << std::endl;
				DeleteDirectory(subdir);
			}
		}
		else
		{
			std::cerr << "Error getting file or directory " << subdir << std::endl;
		}
	}
	closedir(directory);
	std::cout << "Path -> " << path << std::endl;
	if (rmdir(path) == 0)
	{
		std::cout << path << ": Directory deleted successfully" << std::endl;
	}
	else
	{
		std::cerr << "Error deleting directory " << path << std::endl;
	}
	return;
}

void Handler::DeleteFile(const char *path)
{
	if (unlink(path) == 0)
		std::cout << path << ": File deleted successfully" << std::endl;
	else
		perror("Unlink -> ");
}

void Handler::HandleDelete()
{
	struct stat file;
	std::string path = this->_path;

	//path = GetRootLocation(_uri, this->_workingLocation.GetLocationPath(), this->_workingLocation.GetRoot());


	if (stat(path.c_str(), &file) == 0)
	{
		if (S_ISREG(file.st_mode))
		{
			if (this->_config.GetCgiInfo().type == "n/a")
			{
				DeleteFile(path.c_str());
				sendResponseHeader("204", ".html", "", file.st_size);
				// No CGI
			}
			else
			{
				// Run CGI
			}
		}
		else if (S_ISDIR(file.st_mode))
		{
			if (path[path.size() - 1] == '/')
			{
				if (this->_config.GetCgiInfo().type == "n/a")
				{
					DeleteDirectory(path.c_str());
					sendResponseHeader("204", ".html", "", file.st_size);
					// No Cgi
				}
				else
				{
					//  Check on index file validation
					// if (check index file)
						// Run CGI
					// else
					// {
					// 	sendErrorResponse("403");
					// }
				}
			}
			else
			{
				sendErrorResponse("409");
			}
		}
	}
	else
		std::cerr << "File or Directory doesn't exist :(" << std::endl;
}
