#include "../includes/Shared.hpp"

Exceptions::Exceptions(std::string error_message) : _error_message(error_message)
{
	//Exceptions class constructor
}

const char *Exceptions::what() const throw()
{
	return(_error_message.c_str());
}

Exceptions::~Exceptions() throw()
{
	//Exceptions class destructor
}

Shared::Shared()
{
	// MIME Types
	// mp3, mp4, 

	this->mime_types[".html"] = "text/html";
	this->mime_types[".htm"] = "text/html";
	this->mime_types[".shtml"] = "text/html";
	this->mime_types[".css"] = "text/css";
	this->mime_types[".xml"] = "application/xml";
	this->mime_types[".gif"] = "image/gif";
	this->mime_types[".jpeg"] = "image/jpeg";
	this->mime_types[".jpg"] = "image/jpeg";
	this->mime_types[".js"] = "application/javascript";
	this->mime_types[".atom"] = "application/atom+xml";
	this->mime_types[".rss"] = "application/rss+xml";
	this->mime_types[".mml"] = "text/mathml";
	this->mime_types[".txt"] = "text/plain";
	this->mime_types[".jad"] = "text/vnd.sun.j2me.app-descriptor";
	this->mime_types[".wml"] = "text/vnd.wap.wml";
	this->mime_types[".htc"] = "text/x-component";
	this->mime_types[".png"] = "image/png";
	this->mime_types[".tif"] = "image/tiff";
	this->mime_types[".tiff"] = "image/tiff";
	this->mime_types[".wbmp"] = "image/vnd.wap.wbmp";
	this->mime_types[".ico"] = "image/x-icon";
	this->mime_types[".jng"] = "image/x-jng";
	this->mime_types[".bmp"] = "image/x-ms-bmp";
	this->mime_types[".svg"] = "image/svg+xml";
	this->mime_types[".svgz"] = "image/svg+xml";
	this->mime_types[".webp"] = "image/webp";
	this->mime_types[".woff"] = "application/font-woff";
	this->mime_types[".jar"] = "application/java-archive";
	this->mime_types[".war"] = "application/java-archive";
	this->mime_types[".ear"] = "application/java-archive";
	this->mime_types[".json"] = "application/json";
	this->mime_types[".hqx"] = "application/mac-binhex40";
	this->mime_types[".doc"] = "application/msword";
	this->mime_types[".pdf"] = "application/pdf";
	this->mime_types[".ps"] = "application/postscript";
	this->mime_types[".eps"] = "application/postscript";
	this->mime_types[".ai"] = "application/postscript";
	this->mime_types[".rtf"] = "application/rtf";
	this->mime_types[".m3u8"] = "application/vnd.apple.mpegurl";
	this->mime_types[".kml"] = "application/vnd.google-earth.kml+xml";
	this->mime_types[".kmz"] = "application/vnd.google-earth.kmz";
	this->mime_types[".xls"] = "application/vnd.ms-excel";
	this->mime_types[".eot"] = "application/vnd.ms-fontobject";
	this->mime_types[".ppt"] = "application/vnd.ms-powerpoint";
	this->mime_types[".odg"] = "application/vnd.oasis.opendocument.graphics";
	this->mime_types[".odp"] = "application/vnd.oasis.opendocument.presentation";
	this->mime_types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->mime_types[".odt"] = "application/vnd.oasis.opendocument.text";
	this->mime_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	this->mime_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	this->mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	this->mime_types[".wmlc"] = "application/vnd.wap.wmlc";
	this->mime_types[".7z"] = "application/x-7z-compressed";
	this->mime_types[".cco"] = "application/x-cocoa";
	this->mime_types[".jardiff"] = "application/x-java-archive-diff";
	this->mime_types[".jnlp"] = "application/x-java-jnlp-file";
	
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