# Webserv
Making an HTTP WebServer in C from scratch!

OOP in our Webserver :

	class Server 
  	{
	// Setuping and startig the server : Creating socket -> binding -> listening (Handling multiple clients)
	}
  
  	class Handler
  	{
	// Parse request -> validate request content -> match methed --> send response
	}
	
	class Client
  	{
	// Everything related to each client : ...(not sure yet)
  	}

	class Shared
  	{
	// Shared resources such as mime-types map, status codes map or other data structures that are used by multiple parts of the server.
  	}



# Resources

https://www.gta.ufrj.br/ensino/eel878/sockets/index.html

https://stackoverflow.com/questions/6729366/what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming

https://stackoverflow.com/questions/76412255/how-to-fix-error-141-when-using-select-and-send-in-c-web-server-for-multip/76413445

https://stackoverflow.com/questions/5815675/what-is-sock-dgram-and-sock-stream



## TIPS:

if you want to look for a manuel of any function use this website mkssoftware/qnx (eg: mkssoftware select)