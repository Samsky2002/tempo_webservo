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
		if ( clientInfo[i].get_id() == id )
		{
			clientInfo[i].requestSetup( buffer );
		}
	}
}

void Client::responseSetup( int id, int &kq )
{
	for ( size_t i = 0; i < clientInfo.size(); i++ )
	{
		if ( clientInfo[i].get_id() == id )
		{
			clientInfo[i].responseSetup( kq );
		}
	}
}

std::string Client::result( int id )
{
	for ( size_t i = 0; i < clientInfo.size(); i++ )
	{
		if ( clientInfo[i].get_id() == id )
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
		if ( id == clientInfo[ i ].get_id() )
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
		if ( clientInfo[ i ].get_id() == id )
			return ( i );
		i++;
	}
	return ( i );
}

bool Client::isCgiWrite( int fd ) {
	for ( size_t i = 0; i < clientInfo.size(); i++ ) {
		if ( clientInfo[ i ].get_response().get_cgi().get_input_fd() == fd ) {
			return ( true );
		}
	}
	return ( false );
}

void Client::fillCgiWrite( int fd, int &kq ) {
	for ( size_t i = 0; i < clientInfo.size(); i++ ) {
		if ( clientInfo[ i ].get_response().get_cgi().get_input_fd() == fd ) {
			clientInfo[ i ].get_response().get_cgi().fill_input( kq );
		}
	}
}

void Client::fillCgiRead( int fd, int &kq ) {
	for ( size_t i = 0; i < clientInfo.size(); i++ ) {
		if ( clientInfo[ i ].get_response().get_cgi().get_output_fd() == fd ) {
			clientInfo[ i ].get_response().get_cgi().fill_output( kq );
		}
	}
}

bool Client::isCgiRead( int fd ) {
	for ( size_t i = 0; i < clientInfo.size(); i++ ) {
		if ( clientInfo[ i ].get_response().get_cgi().get_output_fd() == fd ) {
			return ( true );
		}
	}
	return ( false );
}

bool Client::response_status( int id ) {
	for ( size_t i = 0; i < clientInfo.size(); i++ ) {
		if ( clientInfo[ i ].get_id() == id	 ) {
			return ( clientInfo[ i ].get_response_status() );
		}
	}
	return ( false ); 
}
// get_response and get_cgi, i need the original not the copy
// because i am modifiying the original
// maybe change the way you call this method
