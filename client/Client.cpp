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

void Client::responseSetup( int id, fd_set & tmp_readfds, fd_set & tmp_writefds, int & nfds )
{
	for ( size_t i = 0; i < clientInfo.size(); i++ )
	{
		if ( clientInfo[i].get_id() == id )
		{
			clientInfo[i].responseSetup( tmp_readfds, tmp_writefds, nfds );
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

void Client::fillCgiWrite( int fd, fd_set & tmp_writefds ) {
	for ( size_t i = 0; i < clientInfo.size(); i++ ) {
		if ( clientInfo[ i ].get_response().get_cgi().get_input_fd() == fd ) {
			clientInfo[ i ].get_response().get_cgi().fill_input( tmp_writefds );
		}
	}
}

void Client::fillCgiRead( int fd, fd_set & tmp_readfds ) {
	for ( size_t i = 0; i < clientInfo.size(); i++ ) {
		if ( clientInfo[ i ].get_response().get_cgi().get_output_fd() == fd ) {
			clientInfo[ i ].get_response().get_cgi().fill_output( tmp_readfds );
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

void Client::clear( int id ) {
    for ( size_t i = 0; i < clientInfo.size(); i++ ) {
        if ( clientInfo[ i ].get_id() == id ) {
            clientInfo[ i ].clear();
            return ;
        }
    }

}
// get_response and get_cgi, i need the original not the copy
// because i am modifiying the original
// maybe change the way you call this method
