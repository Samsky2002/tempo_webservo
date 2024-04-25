#ifndef MULTIPLEX_HPP
# define MULTIPLEX_HPP
# include "Request.hpp"
# include "Response.hpp"
# include "Client.hpp"

class Multiplex
{
	public:
		Client client;
		int nfds;
		int newfd;
		int rec;
		int sen;
		char recv_buffer[1024];
		std::vector<char> buffer;
		std::string send_buffer;
		std::string req;
		fd_set readfds;
		fd_set writefds;
		fd_set tmp_readfds;
		fd_set tmp_writefds;
		std::vector<int> socket_list;
		Multiplex();
		Multiplex( const Multiplex & multiplex );
		Multiplex & operator=( const Multiplex & multiplex );
		~Multiplex();
		void execute( const Server & server );
		void setup( const std::vector< ServerInfo > & serverInfo );
		void multiplexing( const Server & server );
		bool is_socket( const std::vector< ServerInfo > & serverInfo, int socket );
};

#endif
