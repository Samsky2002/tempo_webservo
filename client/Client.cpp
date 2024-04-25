#include "webserv.hpp"

Client::Client()
{

}

Client::Client(const Client & client)
{
	*this = client;
}

Client & Client::operator=(const Client & client)
{
	if ( this != &client )
	{
		//std::cout << "copy assignment client\n";
		this->clientInfo = client.clientInfo;
	}
	return ( *this );
}

Client::~Client()
{

}

void Client::requestSetup( std::vector<char> & buffer, int id )
{
	for ( size_t i = 0; i < clientInfo.size(); i++ )
	{
		if ( clientInfo[i].id == id )
		{
			clientInfo[i].requestSetup( buffer );
		}
	}
}

void Client::responseSetup( int id )
{
	for ( size_t i = 0; i < clientInfo.size(); i++ )
	{
		if ( clientInfo[i].id == id )
		{
			clientInfo[i].responseSetup();
		}
	}
}

std::string Client::result( int id )
{
	for ( size_t i = 0; i < clientInfo.size(); i++ )
	{
		if ( clientInfo[i].id == id )
		{
			return ( clientInfo[i].result() );
		}
	}
	return ( std::string() );
}

void Client::removeClient( int id )
{
	for ( size_t i = 0; i < clientInfo.size(); i++ )
	{
		if ( id == clientInfo[ i ].id )
		{
			clientInfo.erase( clientInfo.begin() + i );
			return ;
		}
	}
}

void Client::addClient( const ClientInfo & newClientInfo )
{
	clientInfo.push_back( newClientInfo );
}

int Client::getClientInfoIndex( int id )
{
	size_t i = 0;

	while ( i < clientInfo.size() )
	{
		if ( clientInfo[ i ].id == id )
			return ( i );
		i++;
	}
	return ( i );
}
