#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "ClientInfo.hpp"
# include <vector>

class Client
{
	public:
		std::vector<ClientInfo> clientInfo;
		Client();
		Client(const Client & client);
		Client & operator=(const Client & client);
		~Client();
		int getClientInfoIndex( int id );
		void addClient( const ClientInfo & newClientInfo );
		void removeClient( int id );
		void requestSetup( std::vector<char> & buffer, int id );
		void responseSetup( int id );
		std::string result( int id );
};

#endif
