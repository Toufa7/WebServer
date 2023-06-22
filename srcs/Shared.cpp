#include "../includes/Shared.hpp"

Shared::Shared()
{
	this->file_extensions["text/html"]= ".html";
	this->file_extensions["text/css"]= ".css";
	this->file_extensions["application/javascript"]= ".js";
	this->file_extensions["application/json"]= ".json";
	this->file_extensions["application/xml"]= ".xml";
	this->file_extensions["text/plain"]= ".txt";
	this->file_extensions["image/jpeg"]= ".jpg";
	this->file_extensions["image/png"]= ".png";
	this->file_extensions["image/gif"]= ".gif";
	this->file_extensions["image/bmp"]= ".bmp";
	this->file_extensions["image/x-icon"]= ".ico";
	this->file_extensions["application/pdf"]= ".pdf";
	this->file_extensions["application/zip"]= ".zip";
	this->file_extensions["application/x-tar"]= ".tar";
	this->file_extensions["application/gzip"]= ".gz";
	this->file_extensions["application/x-rar-compressed"]= ".rar";
	this->file_extensions["application/x-7z-compressed"]= ".7z";
	this->file_extensions["audio/mpeg"]= ".mp3";
	this->file_extensions["audio/wav"]= ".wav";
	this->file_extensions["video/mp4"]= ".mp4";
	this->file_extensions["video/x-msvideo"] = ".avi";
	this->file_extensions["application/vnd.ms-powerpoint"] = ".ppt";
	this->file_extensions["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
	this->file_extensions["application/msword"] = ".doc";
	this->file_extensions["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
	this->file_extensions["application/vnd.ms-excel"] = ".xls";
	this->file_extensions["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
	this->file_extensions["text/csv"] = ".csv";
	this->file_extensions["application/x-shockwave-flash"] = ".swf";
	this->file_extensions["image/svg+xml"] = ".svg";
	this->file_extensions["video/mpeg"] = ".mpg";
	this->file_extensions["video/webm"] = ".webm";
	this->file_extensions["audio/ogg"] = ".ogg";
	this->file_extensions["video/ogg"] = ".ogg";
	this->file_extensions["image/webp"] = ".webp";
	this->file_extensions["image/tiff"] = ".tif";
	this->file_extensions["application/font-woff"] = ".woff";
	this->file_extensions["application/font-woff2"] = ".woff2";
	this->file_extensions["application/x-font-ttf"] = ".ttf";
	this->file_extensions["application/x-font-opentype"] = ".otf";
	this->file_extensions["application/vnd.ms-fontobject"] = ".eot";
	this->file_extensions["application/octet-stream"] = ".bin";
	this->file_extensions["application/x-perl"] = ".pl";
	this->file_extensions["application/x-httpd-php"] = ".php";
	this->file_extensions["video/quicktime"] = ".mov";

	// MIME Types
	this->mime_types[".html"]	= "text/html";
	this->mime_types[".htm"]	= "text/html";
	this->mime_types[".css"]	= "text/css";
	this->mime_types[".js"]		= "application/javascript";
	this->mime_types[".json"]	= "application/json";
	this->mime_types[".xml"]	= "application/xml";
	this->mime_types[".txt"]	= "text/plain";
	this->mime_types[".jpg"]	= "image/jpeg";
	this->mime_types[".jpeg"]	= "image/jpeg";
	this->mime_types[".png"]	= "image/png";
	this->mime_types[".gif"]	= "image/gif";
	this->mime_types[".bmp"]	= "image/bmp";
	this->mime_types[".ico"]	= "image/x-icon";
	this->mime_types[".pdf"]	= "application/pdf";
	this->mime_types[".zip"]	= "application/zip";
	this->mime_types[".tar"]	= "application/x-tar";
	this->mime_types[".gz"]		= "application/gzip";
	this->mime_types[".rar"]	= "application/x-rar-compressed";
	this->mime_types[".7z"]		= "application/x-7z-compressed";
	this->mime_types[".mp3"]	= "audio/mpeg";
	this->mime_types[".wav"]	= "audio/wav";
	this->mime_types[".mp4"]	= "video/mp4";
	this->mime_types[".avi"]	= "video/x-msvideo";
	this->mime_types[".ppt"]	= "application/vnd.ms-powerpoint";
	this->mime_types[".pptx"]	= "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	this->mime_types[".doc"]	= "application/msword";
	this->mime_types[".docx"]	= "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	this->mime_types[".xls"]	= "application/vnd.ms-excel";
	this->mime_types[".xlsx"]	= "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	this->mime_types[".csv"]	= "text/csv";
	this->mime_types[".swf"]	= "application/x-shockwave-flash";
	this->mime_types[".svg"]	= "image/svg+xml";
	this->mime_types[".mpg"]	= "video/mpeg";
	this->mime_types[".mpeg"]	= "video/mpeg";
	this->mime_types[".webm"]	= "video/webm";
	this->mime_types[".ogg"]	= "audio/ogg";
	this->mime_types[".ogg"]	= "video/ogg";
	this->mime_types[".webp"]	= "image/webp";
	this->mime_types[".tif"]	= "image/tiff";
	this->mime_types[".tiff"]	= "image/tiff";
	this->mime_types[".woff"]	= "application/font-woff";
	this->mime_types[".woff2"]	= "application/font-woff2";
	this->mime_types[".ttf"]	= "application/x-font-ttf";
	this->mime_types[".otf"]	= "application/x-font-opentype";
	this->mime_types[".eot"]	= "application/vnd.ms-fontobject";
	this->mime_types[".bin"]	= "application/octet-stream";
	this->mime_types[".php"]	= "application/x-httpd-php";
	this->mime_types[".pl"]		= "application/x-perl";
	this->mime_types[".mov"]	= "video/quicktime";

	// Status codes
	this->status_codes["100"] = "Continue";
	this->status_codes["101"] = "Switching Protocols";
	this->status_codes["102"] = "Processing";
	this->status_codes["200"] = "OK";
	this->status_codes["201"] = "Created";
	this->status_codes["202"] = "Accepted";
	this->status_codes["203"] = "Non-Authoritative Information";
	this->status_codes["204"] = "No Content";
	this->status_codes["205"] = "Reset Content";
	this->status_codes["206"] = "Partial Content";
	this->status_codes["207"] = "Multi-Status";
	this->status_codes["208"] = "Already Reported";
	this->status_codes["226"] = "IM Used";
	this->status_codes["300"] = "Multiple Choices";
	this->status_codes["301"] = "Moved Permanently";
	this->status_codes["302"] = "Found";
	this->status_codes["303"] = "See Other";
	this->status_codes["304"] = "Not Modified";
	this->status_codes["305"] = "Use Proxy";
	this->status_codes["307"] = "Temporary Redirect";
	this->status_codes["308"] = "Permanent Redirect";
	this->status_codes["400"] = "Bad Request";
	this->status_codes["401"] = "Unauthorized";
	this->status_codes["402"] = "Payment Required";
	this->status_codes["403"] = "Forbidden";
	this->status_codes["404"] = "Not Found";
	this->status_codes["405"] = "Method Not Allowed";
	this->status_codes["406"] = "Not Acceptable";
	this->status_codes["407"] = "Proxy Authentication Required";
	this->status_codes["408"] = "Request Timeout";
	this->status_codes["409"] = "Conflict";
	this->status_codes["410"] = "Gone";
	this->status_codes["411"] = "Length Required";
	this->status_codes["412"] = "Precondition Failed";
	this->status_codes["413"] = "Payload Too Large";
	this->status_codes["414"] = "URI Too Long";
	this->status_codes["415"] = "Unsupported Media Type";
	this->status_codes["416"] = "Range Not Satisfiable";
	this->status_codes["417"] = "Expectation Failed";
	this->status_codes["418"] = "I'm a teapot";
	this->status_codes["421"] = "Misdirected Request";
	this->status_codes["422"] = "Unprocessable Entity";
	this->status_codes["423"] = "Locked";
	this->status_codes["424"] = "Failed Dependency";
	this->status_codes["425"] = "Too Early";
	this->status_codes["426"] = "Upgrade Required";
	this->status_codes["428"] = "Precondition Required";
	this->status_codes["429"] = "Too Many Requests";
	this->status_codes["431"] = "Request Header Fields Too Large";
	this->status_codes["451"] = "Unavailable For Legal Reasons";
	this->status_codes["500"] = "Internal Server Error";
	this->status_codes["501"] = "Not Implemented";
	this->status_codes["502"] = "Bad Gateway";
	this->status_codes["503"] = "Service Unavailable";
	this->status_codes["504"] = "Gateway Timeout";
	this->status_codes["505"] = "HTTP Version Not Supported";
	this->status_codes["506"] = "Variant Also Negotiates";
	this->status_codes["507"] = "Insufficient Storage";
	this->status_codes["508"] = "Loop Detected";
	this->status_codes["510"] = "Not Extended";
	this->status_codes["511"] = "Network Authentication Required";
}


std::string Shared::generateFileName(const std::string& path, const std::string& fileExtension)
{
	std::string fileName = path;
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    // Generate a random string for the file name
    for (int i = 0; i < 10; ++i)
        fileName += characters[std::rand() % characters.length()];

    fileName += fileExtension;

    return fileName;
}

std::string Shared::fileExtention(std::string fileName)
{
	std::string extention;
	int	dot_position = fileName.find_last_of('.');
    
	if (dot_position > 0)
	{
        extention = fileName.substr(dot_position ,fileName.length());
		return (extention);
	}
	else
		return ("");
}