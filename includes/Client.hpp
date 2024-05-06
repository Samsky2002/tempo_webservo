#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "ClientInfo.hpp"
# include <vector>

class Client
{
	private:
		std::vector<ClientInfo> clientInfo;
	public:
		Client();
		Client(const Client & client);
		Client & operator=(const Client & client);
		~Client();
		int getClientInfoIndex( int id );
		void addClient( const ClientInfo & newClientInfo );
		void removeClient( int id );
		void requestSetup( std::vector<char> & buffer, int id );
		void responseSetup( int id, fd_set & tmp_readfds, fd_set & tmp_writefds, int & nfds);
		std::string result( int id );
		bool isCgiWrite( int fd );
		void fillCgiWrite( int fd, fd_set & tmp_writefds );
		void fillCgiRead( int fd, fd_set & tmp_readfds );
		bool isCgiRead( int fd );
		bool response_status( int id );
		void clear( int id );
};

#endif
