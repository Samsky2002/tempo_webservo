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

std::vector<std::string> split(const std::string & str, char c);
#endif
