#include "webserv.hpp"

ServerInfo::ServerInfo()
{
	
}

ServerInfo::ServerInfo( const ServerInfo & serverInfo )
{
	*this = serverInfo;
}

ServerInfo & ServerInfo::operator=( const ServerInfo & serverInfo )
{
	if ( this != &serverInfo )
	{
		this->id = serverInfo.id;
		this->host = serverInfo.host;
		this->port = serverInfo.port;
		this->serverConfig = serverInfo.serverConfig;
	}
	return ( *this );
}

ServerInfo::~ServerInfo()
{

}

int ServerInfo::get_id() const {
	return ( id );
}

int ServerInfo::get_port() const {
	return ( port );
}

std::string ServerInfo::get_host() const {
	return ( host );
}

void ServerInfo::set_serverConfig( ServerConfig serverConfig ) {
	this->serverConfig.push_back( serverConfig );
}

ServerConfig ServerInfo::get_serverConfig_idx( size_t idx ) const {
	if ( idx >= serverConfig.size() ) {
		throw ( std::string( "get_serverConfig_idx: out_of_range" ) );
	}
	return ( serverConfig[ idx ] );
}

std::vector< ServerConfig > ServerInfo::get_serverConfig() const {
	return ( serverConfig );
}

void ServerInfo::launch()
{
	SocketServer socketServer( port, host );

	socketServer.launch( id );
}

void ServerInfo::print() {
	std::cout << "id: " << id << std::endl;
	std::cout << "host: " << host << std::endl;
	std::cout << "port: " << port << std::endl;
	for ( size_t i = 0; i < serverConfig.size(); i++ ) {
		std::cout << "####################ServerConfig--BEG####################\n";
		serverConfig.at( i ).print();
		std::cout << "####################ServerConfig--END####################\n";
	}
}

void ServerInfo::clear() {
	id = -1;
	port = -1;
	host.clear();
	serverConfig.clear();
}

// maybe do a tokenizer and check
void ServerInfo::check_host( const std::string & strHost ) {
	std::vector< std::string > values;
	int nb = 0;
	std::stringstream ss;
	int dotCount = 0;

	values = split( strHost, '.' );
	for ( size_t i = 0; i < strHost.size(); i++ ) {
		if ( strHost.at( i ) == '.' ) {
			dotCount++;
		}
	}
	if ( dotCount != 3 ) {
		throw ( std::string( "invalid host" ) );
	}
	if ( values.size() != 4 )
		throw ( std::string( "invalid host" ) );
	for ( size_t i = 0; i < values.size(); i++ ) {
		ss << values.at( i );
		ss >> nb;
		if (ss.fail() || !ss.eof()) {
			throw ( std::string( "invalid port" ) );
		}
		if ( nb < 0 || nb > 255 ) {
			throw ( std::string( "invalid port" ) );
		}
		ss.clear();
	}
	host = strHost;
}

void ServerInfo::check_port( const std::string & strPort ) {
	std::stringstream ss;
	int nbPort;

	ss << strPort;
	ss >> nbPort;
	if (ss.fail() || !ss.eof()) {
		throw ( std::string( "invalid port" ) );
    }
	if ( nbPort < 0 || nbPort > 65535 )
		throw ( std::string( "invalid port" ) );
	port = nbPort;
}

void ServerInfo::fill_host_port( const std::vector< std::string > & directive ) {
	std::string host;
	std::string port;
	size_t pos;

	if ( directive.size() != 2 ) {
		throw ( std::string( "listen directive" ) );
	}
	pos = directive.at( 1 ).find(":");
	if ( pos == std::string::npos ) {
		throw ( std::string( "listen directive" ) );
	}
	host = directive.at( 1 ).substr( 0, pos );
	check_host( host );
	port = directive.at( 1 ).substr( pos + 1 );
	check_port( port );
}

void ServerInfo::fill( const std::vector< std::vector< std::string > > & directives ) {
	for ( size_t i = 0; i < directives.size(); i++ ) {
		if ( directives.at( i ).at( 0 ) == "listen" ) {
			fill_host_port( directives.at( i ) );
			return ;
		}
	}
}

// you can konw by the id of the serverInfo if it's a socket or not
