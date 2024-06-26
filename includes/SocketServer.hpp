#ifndef SOCKETSERVER_HPP
# define SOCKETSERVER_HPP
# include <arpa/inet.h>

class SocketServer
{
	private:
		// for sockaddr_in
		int port;
		std::string host;
		int family;
		struct sockaddr_in serverAddr;
		socklen_t addrLen;
		// for socket
		int domain;
		int type;
		int protocol;
		int listener;
		// for setsockopt
		int level;
		int option_name;
		int option_value;
		socklen_t option_len;
		// for listen
		int backlog;
	public:
		SocketServer();
		SocketServer( int port, std::string host );
		SocketServer( const SocketServer & socketServer );
		SocketServer & operator=( const SocketServer & socketServer );
		~SocketServer();
		void launch( int & id );
};
#endif
