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
		void responseSetup( int id, int &kq );
		std::string result( int id );
		bool isCgiWrite( int fd );
		void fillCgiWrite( int fd, int &kq );
		void fillCgiRead( int fd, int &kq );
		bool isCgiRead( int fd );
		bool response_status( int id );
};

#endif
