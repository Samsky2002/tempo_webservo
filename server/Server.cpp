#include "webserv.hpp"

Server::Server()
{

}

Server & Server::operator=( const Server & server )
{
	if ( this != &server )
	{
		this->serverInfo = server.serverInfo;
	}
	return ( *this );
}

Server::~Server()
{

}
// launch function

void Server::execute( Parser & parser )
{
	setup( parser );
	launch();
}

void Server::fill_server_info() {
	for ( size_t i = 0; i < serverInfo.size(); i++ ) {
		if ( serverInfo.at( i ).host == tmpServerInfo.host && serverInfo.at( i ).port == tmpServerInfo.port ) {
			serverInfo.at( i ).serverConfig.push_back( tmpServerInfo.serverConfig.at( 0 ) );		
			return ;
		}
	}
	serverInfo.push_back( tmpServerInfo );	
}

void Server::fill( const std::vector< ServerContext > & serverContext ) {
	for ( size_t i = 0; i < serverContext.size(); i++ ) {
		tmpServerInfo.fill( serverContext.at( i ).serverDirectives );
		tmpServerConfig.fill( serverContext.at( i ).serverDirectives );
		for ( size_t j = 0; j < serverContext.at( i ).locationContext.size(); j++ ) {
			tmpLocation.fill( serverContext.at( i ).locationContext.at( j ).locationDirectives );
			tmpServerConfig.location.push_back( tmpLocation );
			tmpLocation.clear();
		}
		tmpServerInfo.serverConfig.push_back( tmpServerConfig );
		tmpServerConfig.clear();
		fill_server_info();
		tmpServerInfo.clear();
	}
}

void Server::setup( Parser & parser )
{
	try {
		fill( parser.serverContext );
	}
	catch ( const std::string & e ) {
		std::cout << "syntax error: " << e << std::endl;
		exit(0);
	}
	//print();
	// parse and stuff
}

void Server::launch()
{
	for ( size_t i = 0; i < serverInfo.size(); i++ )
	{
		serverInfo[i].launch();
	}
}

ServerInfo Server::getServer( int id ) const
{
	size_t i = 0;

	while ( i < serverInfo.size() )
	{
		if ( id == serverInfo[i].id)
			break ;
		i++;
	}
	return ( serverInfo[i] );
}

void Server::clear() {
	serverInfo.clear();
}

void Server::print() {
	for ( size_t i = 0; i < serverInfo.size(); i++ ) {
		std::cout << "----------------------------ServerInfo-BEG--------------------\n";
		serverInfo.at( i ).print();
		std::cout << "----------------------------ServerInfo-END--------------------\n";
	}
}
