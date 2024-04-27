#ifndef SOCKETSERVER_HPP
# define SOCKETSERVER_HPP
# include <arpa/inet.h>

class SocketServer
{
	private:
		// for sockaddr_in
		int					port;
		int					family;
		std::string 		host;
		struct sockaddr_in	serverAddr;

		// struct sockaddr_in clientAddr; We don't use it (To Remove)
		socklen_t			addrLen;
		// for socket
		int					type;
		int					domain;
		int					protocol;
		int					listener;
		// for setsockopt
		int					level;
		socklen_t			option_len;
		int					option_name;
		int					option_value;
		// for listen
		int					backlog;

		public:
		SocketServer();
		SocketServer( int port, std::string host );
		SocketServer( const SocketServer & socketServer );
		SocketServer & operator=( const SocketServer & socketServer );
		~SocketServer();

		//Method
		void launch( int & id );
};
#endif
