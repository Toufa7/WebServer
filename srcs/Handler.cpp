#include "../includes/Handler.hpp"

//  ------------- CONSTRUCTOR && DESTRUCTOR --------------------

void Handler::setData()
{
	this->_postFileFd = -1;
	this->_headerflag = 0;
	this->_chunkSize = 0;
	this->_postRecv = 0;
	this->_chunkHexState = 0;
	this->_cgiPid = -1;
}

//  ------------- ACCESSOR --------------------

void Handler::setConfig(ServerConfig &config)
{
	this->_config = config;
}

// ------------- METHODS -------------

int Handler::Driver(char *requested_data, int bytesreceived)
{
	int re = 1;
	if (this->_headerflag == 0)
		re = this->parseRequestHeader(requested_data, bytesreceived);
	else
	{
		if (this->_method == "GET")
			re = this->HandleGet();
		else if (this->_method == "POST")
			re = this->HandlePost(requested_data, bytesreceived);
		else if (this->_method == "DELETE")
			re = this->HandleDelete();
	}
	this->_headerflag = 1;
	return re;
}

std::string Handler::GetRootLocation(std::string uri, std::string locationPath, std::string root)
{
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

// Generate a response header from the recieved argements
void Handler::SendResponseHeader(std::string statusCode, std::string fileExt, std::string location, int contentLength)
{
	//
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
	{
		perror("Error : Send <Response Header>  -> ");
	}
}

std::string Handler::generateListDir(std::string statusCode, std::string ls)
{
	if (ls.empty() == 1) // if an error happened, check ls wether is empty or not
	{
		perror("Error : GenerateListDir  <Directory listing> ");
		this->sendCodeResponse("500");
	}

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

void Handler::sendCodeResponse(std::string statusCode)
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
	SendResponseHeader(statusCode, ".html", "", htmlContent.length());
	if (send(this->client_socket, htmlContent.c_str(), htmlContent.length(), 0) == -1)
	{
		perror("Error : Send <Error Response> ");
	}
}

// -------------------------------- Request parse and validation ------------

int Handler::parseRequestHeader(char *req, int bytesreceived)
{
	int delimiter_position;
	std::string current_line, key, value;
	char *body;

	std::string request = req;
	size_t header_len = request.find("\r\n\r\n"); // Find the end of the request header
	if (header_len == std::string::npos)
	{
		perror("Error : ParseRequstHeader <Finding end of request> ");
		this->sendCodeResponse("500");
		return 0;
	}
	std::string header = request.substr(0, header_len);			  // Save the header
	body = req + header_len + 4, bytesreceived -= header_len + 4; // Save the body
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
		// storing key and value in map
		this->_req_header[key] = value;
	}
	
	if (!this->ValidateRequest())
		return 0;

	if (this->_method == "GET")
		return this->HandleGet();
	else if (this->_method == "POST")
		return this->HandlePost(body, bytesreceived);
	else if (this->_method == "DELETE")
		return this->HandleDelete();
	return 0;
}

// Check for Possible error in the Request
bool Handler::ValidateRequest()
{
	// if Transfer-Encoding exist and not match [chunked]
	if (this->_req_header.find("Transfer-Encoding") != this->_req_header.end() && this->_req_header["Transfer-Encoding"] != "chunked")
	{
		this->sendCodeResponse("501");
		return false;
	}
	// if both Transfer-Encoding and Content-Length not provided
	if (this->_method == "POST" && this->_req_header.find("Transfer-Encoding") == this->_req_header.end() &&
		this->_req_header.find("Content-Length") == this->_req_header.end())
	{
		this->sendCodeResponse("400");
		return false;
	}
	// URI should start with a leading slash ("/") and not contain any illegal characters
	if (!this->ValidateURI(this->_uri))
	{
		this->sendCodeResponse("400");
		return false;
	}
	//  URI should not have more than 2048
	if (this->_uri.length() > 2048)
	{
		this->sendCodeResponse("414");
		return false;
	}
	//  Request body size should not be more than [client_body_size] from confing file
	if (this->_req_header.find("Content-Length") != this->_req_header.end() &&
		std::stoll(this->_req_header["Content-Length"]) > std::stoll(this->_config.GetClientBodySize()))
	{
		this->sendCodeResponse("413");
		return false;
	}
	return this->MatchLocation();
}

bool fn(ServerLocation &x, ServerLocation &y)
{
    return x.GetLocationPath().length() > y.GetLocationPath().length();
}

// match location from the config file and validate method
bool Handler::MatchLocation()
{
	this->_path = this->_uri;
	std::vector<ServerLocation> serverLocations = this->_config.GetLocationsVec();
	size_t i = 0; 

	// Seperate Path from args if there is any
	if (this->_uri.find('?') != std::string::npos)
	{
		_path = this->_uri.substr(0, this->_uri.find('?'));
		_querystring = this->_uri.substr(this->_uri.find('?') + 1, this->_uri.length());
	}

	std::sort(serverLocations.begin(), serverLocations.end(), fn);

	// find the closest location to requested resource (new)
	for (i = 0; i < serverLocations.size(); i++)
	{
		std::string locationslash;

		size_t j = 0;
		while (j < _path.size() && j < serverLocations[i].GetLocationPath().size() && 
				_path[j] == serverLocations[i].GetLocationPath()[j])
			j++;
		
		if ((j == _path.size() && j == serverLocations[i].GetLocationPath().size())
			|| (j == serverLocations[i].GetLocationPath().size() && ((j < _path.size() && _path[j] == '/') || (j > 0 && _path[j - 1] == '/'))))
		{
			this->_workingLocation = serverLocations[i];
			break;
		}
	}
	if (i == serverLocations.size())
	{
		this->sendCodeResponse("404");
		return (0);
	}
	

	// Check for location redirection
	if (this->_workingLocation.GetRedirectionInfo().RedirectionFlag)
	{
		_path = this->_workingLocation.GetRedirectionInfo().RedirectionPath;
		this->SendResponseHeader(this->_workingLocation.GetRedirectionInfo().RedirectionCode, "", _path, 0);
		return false;
	}

	std::vector<std::string> allowedMethods = this->_workingLocation.GetAllowedMethodsVec();
	if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_method) == allowedMethods.end())
	{
		this->sendCodeResponse("405");
		return false;
	}

	_path = GetRootLocation(this->_path, this->_workingLocation.GetLocationPath(), this->_workingLocation.GetRoot());
	return true;
}

// Check for any any illegal characters in the URI
bool Handler::ValidateURI(const std::string &uri)
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

int Handler::chunkedPost(char *body, int bytesreceived)
{
	// _chunkHexStates:
	// 0: Still in the \r\n before the hex
	// 1: in the hex number
	// 2: in the \r\n after the hex


	if (this->_chunkHexState == 2)
	{
		int i = 0;
		while (this->_chunkHexState == 2 && i < bytesreceived && (body[i] == '\r' || body[i] == '\n'))
			i++;
		this->_chunkHexState = 0;

		if (bytesreceived - i <= 0)
			return 1;

		body += i, bytesreceived -= i;
	}
	if (this->_chunkSize <= 0)
	{
		int i = 0;

		while (this->_chunkHexState == 0 && i < bytesreceived && (body[i] == '\r' || body[i] == '\n'))
			i++;

		if (i == bytesreceived && body[i - 1] == '\n')
			this->_chunkHexState = 1;
		else
		{
			for (; i < bytesreceived && body[i] != '\r'; i++)
				this->_chunkHex.push_back(body[i]);
			this->_chunkHexState = 1;
			if (i < bytesreceived && body[i] == '\r')
			{
				this->_chunkHexState = 2;
				while (i < bytesreceived && (body[i] == '\r' || body[i] == '\n'))
					i++;
				if ((i < bytesreceived && (body[i] != '\r' || body[i] != '\n')) || (i == bytesreceived && body[i - 1] == '\n'))
					this->_chunkHexState = 0;
			}
		}
		if (this->_chunkHex == "0")
			return 0;
		
		if (this->_chunkHexState == 1)
			return 1;

		std::stringstream ss;
		ss << std::hex << this->_chunkHex;
		ss >> this->_chunkSize;

		this->_chunkHex.clear();
		if (bytesreceived - i <= 0)
			return 1;

		body += i, bytesreceived -= i;
	}

	if (this->_chunkSize >= bytesreceived && bytesreceived > 0)
	{
		write(this->_postFileFd, body, bytesreceived);
		this->_chunkSize -= bytesreceived;
		return 1;
	}
	else if (this->_chunkSize > 0 )
	{
		if (bytesreceived == 0)
			return 1;
		write(this->_postFileFd, body, this->_chunkSize);
		bytesreceived -= this->_chunkSize;
		body += this->_chunkSize;
		this->_chunkSize = 0;
		chunkedPost(body, bytesreceived);
	}
	return 1;
}

int Handler::HandlePost(char *body, int bytesreceived)
{
	if (this->_cgiPid != -1)
	{
		if (this->_workingLocation.GetCgiInfoPhp().type == this->_shared.fileExtention(this->_path))
			return this->HandleCgi(_path, "POST", this->_workingLocation.GetCgiInfoPhp());
		else if (this->_workingLocation.GetCgiInfoPerl().type == this->_shared.fileExtention(this->_path))
			return this->HandleCgi(_path, "POST", this->_workingLocation.GetCgiInfoPerl());
	}

	std::string mimeType = "";
	struct stat s;
	int returnVal = 1;


	// Save MIME type and boundary if it exist
	if (this->_req_header.find("Content-Type") != this->_req_header.end())
	{
		int semiPos = this->_req_header["Content-Type"].find(';');
		if (semiPos != -1)
			mimeType = this->_req_header["Content-Type"].substr(0, semiPos);
		else
			mimeType = this->_req_header["Content-Type"];
	}

	// Create a file stream for writing
	if (this->_headerflag == 0)
	{
		stat(this->_path.c_str(), &s);
		if (S_ISDIR(s.st_mode))
		{
			if (this->_workingLocation.GetUpload() == 0)
			{
				this->sendCodeResponse("403");
				return 0;
			}
			if (_path.back() != '/')
				_path += "/";
			_postFilePath = this->_shared.generateFileName(this->_path, this->_shared.file_extensions[mimeType]);
			this->_postFileFd = open(_postFilePath.c_str(), O_CREAT | O_RDWR | O_APPEND, 0777);
		}
		else
		{
			std::string fileExt = this->_shared.fileExtention(this->_path);
			if ((this->_workingLocation.GetCgiInfoPhp().path != "n/a" && this->_workingLocation.GetCgiInfoPhp().type == fileExt) ||
				(this->_workingLocation.GetCgiInfoPerl().path != "n/a" && this->_workingLocation.GetCgiInfoPerl().type == fileExt))
				{
					_postFilePath = this->_shared.generateFileName("/tmp/", this->_shared.file_extensions[mimeType]);
					this->_postFileFd = open(_postFilePath.c_str(), O_CREAT | O_RDWR | O_APPEND, 0777);
				}
				else
				{
					this->sendCodeResponse("404");
					return 0;
				}
		}
		if (stat(this->_postFilePath.c_str(), &s) != 0)
		{
			this->sendCodeResponse("404");
			return 0;
		}
		if (this->_postFileFd < 0)
		{
			this->sendCodeResponse("500");
			return 0;
		}
	}
	else if (access(_postFilePath.c_str(), F_OK) == -1 || access(_postFilePath.c_str(), W_OK) == -1 || access(_postFilePath.c_str(), R_OK) == -1)
	{
		this->sendCodeResponse("500");
		return 0;
	}


	if (this->_req_header.find("Transfer-Encoding") != this->_req_header.end())
	{
		returnVal = this->chunkedPost(body, bytesreceived);
	}
	else
	{
		long long remmining = std::stoll(this->_req_header["Content-Length"]) - this->_postRecv;
		// Write the request body data to the file
		if (bytesreceived <= remmining)
			write(this->_postFileFd, body, bytesreceived);
		else
			write(this->_postFileFd, body, remmining);

		this->_postRecv += bytesreceived;
		if (this->_postRecv >= std::stoll(this->_req_header["Content-Length"]))
			returnVal = 0;
	}
	
	if (!returnVal)
	{
		stat(this->_path.c_str(), &s);
		if (!S_ISDIR(s.st_mode))
		{
			std::string fileExt = this->_shared.fileExtention(this->_path);
			if (this->_workingLocation.GetCgiInfoPhp().path != "n/a" && this->_workingLocation.GetCgiInfoPhp().type == fileExt)
				returnVal = this->HandleCgi(_path, "POST", this->_workingLocation.GetCgiInfoPhp());
			else if (this->_workingLocation.GetCgiInfoPerl().path != "n/a" && this->_workingLocation.GetCgiInfoPerl().type == fileExt)
				returnVal = this->HandleCgi(_path, "POST", this->_workingLocation.GetCgiInfoPerl());
		}
		else
			this->sendCodeResponse("201");
		close(this->_postFileFd);
	}
	return returnVal;
}

// -------------------------------- GET method ----------------------

int Handler::HandleGet()
{

	if (this->_cgiPid != -1)
	{
		if ((this->_shared.fileExtention(_path) == this->_workingLocation.GetCgiInfoPhp().type))
			return this->HandleCgi(_path, "GET", this->_workingLocation.GetCgiInfoPhp());
				
		if ((this->_shared.fileExtention(_path) == this->_workingLocation.GetCgiInfoPerl().type))
			return this->HandleCgi(_path, "GET", this->_workingLocation.GetCgiInfoPerl());
	}

	// TODO: Function to match the location, takes locationsVec, and uri , and should return the closest match
	int indexfileflag = 0;
	std::string RealPath, tmp_str, DirStr, UriInit;
	struct stat s;

	UriInit = _uri;
	if (stat(_path.c_str(), &s) == 0)
	{
		/*------------------------------------------- DIR Handler ----------------------------------------------------*/
		if (s.st_mode & S_IFDIR)
		{
			if (_path[_path.size() - 1] != '/')
				_path += '/';
			
			if ((s.st_mode & S_IRUSR) == 0)
			{
				this->sendCodeResponse("403");
				return (0);
			}

			if (this->_workingLocation.GetIndexesVec().empty() == 0)
			{
				// case of valid index file so you shuold handle cgi or not
				std::string indexfilepath;

				indexfilepath = _path;
				indexfilepath += this->_workingLocation.GetIndexesVec()[0];
				this->_path = indexfilepath;
				indexfileflag = 1;
			}
			else
			{
				// case of no index file and should check autoindex
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
						if (this->_headerflag == 0)
							SendResponseHeader("200", ".html", "", lsDir.length());
						if (send(this->client_socket, lsDir.c_str(), lsDir.length(), 0) == -1)
						{
							perror("Error : Send <Index Of>	=>	");
							return (0);
						}
						return (0);
					}
				}
				else
				{
					this->sendCodeResponse("403"); // case of no autoindex and no index file
					return (0);
				}
			}
		}
		/*------------------------------------------- DIR Handler end ------------------------------------------------*/

		/*--------------------------------------------- File Handler -------------------------------------------------*/
		if ((s.st_mode & S_IFREG) || (indexfileflag == 1))
		{
			if (this->_workingLocation.GetCgiInfoPhp().path != "n/a" || this->_workingLocation.GetCgiInfoPerl().path != "n/a")
			{
				// handle file cgi
				if ((this->_shared.fileExtention(_path) == this->_workingLocation.GetCgiInfoPhp().type))
				{
					return this->HandleCgi(_path, "GET", this->_workingLocation.GetCgiInfoPhp());
				}
				if ((this->_shared.fileExtention(_path) == this->_workingLocation.GetCgiInfoPerl().type))
				{
					return this->HandleCgi(_path, "GET", this->_workingLocation.GetCgiInfoPerl());
				}
			}
			if (this->_workingLocation.GetCgiInfoPhp().path == "n/a" || this->_workingLocation.GetCgiInfoPerl().path == "n/a" || this->_shared.fileExtention(_path) != this->_workingLocation.GetCgiInfoPhp().type || (indexfileflag == 1)) // regular file, non valid cgi extension and index file present with cgi off
			{
				struct stat file;
				if (this->_headerflag == 0)
				{
					std::string filext = this->_shared.fileExtention(_path);
					requested_file = open(this->_path.c_str(), O_RDONLY);
					if (requested_file < 0)
						perror("Error : Get <Opening file to send it> ");
					stat(this->_path.c_str(), &file);
					SendResponseHeader("200", filext, "", file.st_size);
				}
				bytesread = read(requested_file, buffer, sizeof(buffer));
				if (bytesread == -1)
					perror("Error : Read <Regular File> => ");
				bytessent = send(this->client_socket, buffer, bytesread, 0);
				if (bytessent < 1 || bytesread < CHUNK_SIZE)
				{
					indexfileflag = 0;
					perror("Error : Send <Regular File>  -> ");
					// if (close(requested_file))
					// 	perror("Error : CLOSE <Regular File>");
					return (0);
				}
				return (1);
			}
		}
		/*--------------------------------------------- File Handler end -------------------------------------------------*/
	}
	else
	{
		this->sendCodeResponse("404");
		return (0);
	}
	return (1);
}

// -------------------------------- Delete method ----------------------

int Handler::DeleteDirectory(const char *path)
{
	DIR *directory;
	struct dirent *entry;
	struct stat file;
	char subdir[256];

	if ((directory = opendir(path)) == NULL)
	{
		std::cerr << "Cannot Open Directory: " << path << std::endl;
		sendCodeResponse("403");
		return (1);
	}
	while ((entry = readdir(directory)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		snprintf(subdir, sizeof(subdir), "%s/%s", path, entry->d_name);
		if (stat(subdir, &file) == 0)
		{
			if (S_ISREG(file.st_mode) && (file.st_mode & S_IWUSR))
			{
				DeleteFile(subdir);
			}
			else if (S_ISDIR(file.st_mode))
			{
				DeleteDirectory(subdir);
			}
		}
		else
		{
			std::cerr << "Error getting file or directory " << subdir << std::endl;
			sendCodeResponse("403");
			return 1;
		}
	}
	closedir(directory);
	if (rmdir(path) != 0)
	{
		perror("Error : RMDIR <Deleting Directory> ");
	}
	return (0);
}

void Handler::DeleteFile(const char *path)
{
	if (unlink(path) != 0)
		perror("Error : UNLINK <Delete File>	-> ");
}

int Handler::HandleDelete()
{
	struct stat file;
	std::string path;

	path = this->_path;
	if (stat(path.c_str(), &file) == 0)
	{
		if (S_ISREG(file.st_mode))
		{
			/*
			 * Allowed Permissions
			 */
			if (S_ISREG(file.st_mode) && (file.st_mode & S_IWUSR))
			{
				DeleteFile(path.c_str());
				SendResponseHeader("204", ".html", "", file.st_size);
				return (0);
			}
			/*
				! No permissions
			*/
			else
			{
				sendCodeResponse("403");
				return (0);
			}
		}
		else if (S_ISDIR(file.st_mode))
		{
			if (path[path.size() - 1] == '/')
			{
				/*
				 * Allowed Permissions
				 */
				if (S_ISDIR(file.st_mode) && (file.st_mode & S_IWUSR))
				{
					if (DeleteDirectory(path.c_str()) == 1)
						return (0);
					else
						SendResponseHeader("204", ".html", "", file.st_size);
				}
				/*
					! No permissions
				*/
				else
				{
					sendCodeResponse("403");
					return (0);
				}
			}
			else
			{
				sendCodeResponse("409");
				return (0);
			}
		}
	}
	/*
		! File or Directory doesn't exist
	*/
	else
	{
		sendCodeResponse("404");
		return (0);
	}
	return (0);
}