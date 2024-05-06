
# ifndef Multiplex_HPP
# define Multiplex_HPP

#include "webserv.hpp"
#include "Client.hpp"

#define MAX_SOCKET 1000
class	Multiplex
{
	private:
		//General Elements;
		Client						client;
		std::string 				result;
		int 						newfd;
	 
		// Sets;
		fd_set						readfds;
		fd_set						writefds;
		fd_set						tmp_readfds;
		fd_set						tmp_writefds;

		//Elements For Select;
		int							nfds;
		struct timespec 			timeout;

		//Method
		Multiplex &operator=( const Multiplex& multiplex );
		Multiplex( const Multiplex& multiplex );

		void	HandleCgiRead(int &CgiSocket);
		void	HandleCgiWrite(int &CgiSocket);
		void	HandleClientRead(int &clientSocket);
		void	HandleClientWrite(int &clientSocket);
		void	HandleNewConnection(int &ServerSocket, const Server &server);
		bool	isServer( const std::vector< ServerInfo >& serverInfo, int &socket );

	public:
		Multiplex();
		~Multiplex();
		
		void	execute( const Server & server );
		void	multiplexing( const Server & server );
		void	setup( const std::vector< ServerInfo > & serverInfo );

};

#endif
