# ifndef Multiplex_HPP
# define Multiplex_HPP

#include "webserv.hpp"
#include "Client.hpp"

#define MAX_SOCKET 1000
class	Multiplex
{
	private:
		//General Elements;
		int 						quit;
		Client						client;
		std::string					result;
		std::string					tmp;
	 
		//Elements For Kevents;
		struct timespec 			timeout;
		struct kevent				Changeevent;
		char						recv_buffer[1024];
		std::vector<char>			buffer;
	
	public:
		int 						kq;

		//Method
		Multiplex &operator=( const Multiplex & multiplex );
		Multiplex( const Multiplex & multiplex );

		void	receiving( int &i );
		void	sendig(int &fd );
		void	accepting( int &fdServer, const Server &server ); 
		void	delete_client( int client);
		bool	is_socket( const std::vector< ServerInfo > & serverInfo, int socket );

	public:
		Multiplex();
		~Multiplex();
		
		void	execute( const Server & server );
		void	setup( const std::vector< ServerInfo > & serverInfo );
		void	multiplexing( const Server & server );
	
		class KQUEUE_EXCEPTION : public std::exception
		{
			const char *what() const _NOEXCEPT;
		};
		class KEVENT_EXCEPTION : public std::exception
		{
			const char *what() const _NOEXCEPT;
		};
};

#endif
