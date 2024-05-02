#ifndef WEBSERV_HPP
# define WEBSERV_HPP
# include <iostream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/stat.h>
# include <dirent.h>
# include <cstring>
# include <string>
# include <unistd.h>
# include <fcntl.h>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <sstream>
# include <dirent.h>
# include <sys/event.h>
# include <exception>

# include "Http.hpp"
# include "Server.hpp"
# include "ServerConfig.hpp"
# include "Location.hpp"
# include "SocketServer.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Cgi.hpp"
# include "Multiplex.hpp"
# include "Client.hpp"
# include "ClientInfo.hpp"
# include "Parser.hpp"
# include "ServerContext.hpp"
# include "LocationContext.hpp"

std::vector<std::string>    split(const std::string & str, char c);
int							ConvertHex(const std::string& path) ;
char						HexToChar(const std::string& path);
void						toLower(std::string &key);
void 						parseQuery(std::stringstream& pathStream, std::string& finalPath);
std::string 				decodePath(const std::string& path);
std::vector<std::string> 	split( const std::string str );


/* ************************************Template********************************************** */
template <typename T>
void print(T body)
{
	// Just For asscociative Containers;
	for ( size_t i = 0; i < body.size(); i++ )
		std::cout << body[i];
}

template <typename T>
bool	isExist(T contenair, std::string key) 
{
	// Just For containers that holds std::pair
	for (typename T::iterator it = contenair.begin(); it != contenair.end(); it++)
	{
		if (it->first == key)
			return (true);
	}
	return (false);
}

#endif
