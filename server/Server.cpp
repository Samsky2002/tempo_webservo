#include "webserv.hpp"

Server::Server()
{

}

Server::Server(const Server & server) {
	*this = server;
}

Server & Server::operator=( const Server & server )
{
	if ( this != &server )
	{
		this->serverInfo = server.serverInfo;
		this->tmpServerInfo = server.tmpServerInfo;
		this->tmpServerConfig = server.tmpServerConfig;
		this->tmpLocation = server.tmpLocation;
	}
	return ( *this );
}

Server::~Server()
{

}

ServerInfo Server::get_serverInfo_idx( int id ) const
{
	for ( size_t i = 0; i < serverInfo.size(); i++ ) {
		if ( serverInfo[ i ].get_id() == id )
		{
			return ( serverInfo[ i ] );
		}
	}
	throw ( std::string( "getServer: out_of_range" ) );
}

std::vector< ServerInfo > Server::get_serverInfo() const
{
	return ( serverInfo );
}

void Server::execute( Parser & parser )
{
	setup( parser );
	launch();
}

void Server::fill_server_info() {
	for ( size_t i = 0; i < serverInfo.size(); i++ ) {
		if ( serverInfo[ i ].get_host() == tmpServerInfo.get_host() && serverInfo[ i ].get_port() == tmpServerInfo.get_port() ) {
			serverInfo[ i ].set_serverConfig( tmpServerInfo.get_serverConfig_idx( 0 ) );
			return ;
		}
	}
	serverInfo.push_back( tmpServerInfo );	
}

void Server::fill_location( std::vector< LocationContext > locationContext ) {
	for ( size_t j = 0; j < locationContext.size(); j++ ) {
		tmpLocation.fill( locationContext[ j ].get_locationDirectives() );
		tmpServerConfig.set_location( tmpLocation );
		tmpLocation.clear();
	}
	tmpServerInfo.set_serverConfig( tmpServerConfig );
	tmpServerConfig.clear();
}

void Server::fill( const std::vector< ServerContext > & serverContext ) {
	for ( size_t i = 0; i < serverContext.size(); i++ ) {
		tmpServerInfo.fill( serverContext[ i ].get_serverDirectives() );
		tmpServerConfig.fill( serverContext[ i ].get_serverDirectives() );
		fill_location( serverContext[ i ].get_locationContext() );
		fill_server_info();
		tmpServerInfo.clear();
	}
}

void Server::setup( Parser & parser )
{
	try {
		fill( parser.get_serverContext() );
	}
	catch ( const std::string & e ) {
		std::cout << "syntax error: " << e << std::endl;
		exit(0);
	}
	//print();
}

void Server::launch()
{
	for ( size_t i = 0; i < serverInfo.size(); i++ )
	{
		serverInfo[i].launch();
	}
}


void Server::clear() {
	serverInfo.clear();
}

void Server::print() {
	for ( size_t i = 0; i < serverInfo.size(); i++ ) {
		std::cout << "****************************ServerInfo-BEG****************************\n";
		serverInfo.at( i ).print();
		std::cout << "****************************ServerInfo-END****************************\n";
	}
}
