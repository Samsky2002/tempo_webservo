#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/stat.h>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <dirent.h>
# include <csignal>

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

int							set_nonblocking(int fd);
int							ConvertHex(const std::string& path) ;
char						HexToChar(const std::string& path);
void						toLower(std::string &key);
std::vector<std::string>	split( const std::string str );
std::vector<std::string>	split(const std::string & str, char c);


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
