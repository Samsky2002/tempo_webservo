#include "webserv.hpp"

ServerConfig::ServerConfig()
{
	client_max_body_size = -1;
}


ServerConfig::ServerConfig( const ServerConfig & serverConfig )
{
	*this = serverConfig;
}

ServerConfig & ServerConfig::operator=( const ServerConfig & serverConfig )
{
	if ( this != &serverConfig )
	{
		this->server_names = serverConfig.server_names;
		this->error_pages = serverConfig.error_pages;
		this->client_max_body_size = serverConfig.client_max_body_size;
		this->location = serverConfig.location;
	}
	return (*this);
}

ServerConfig::~ServerConfig()
{

}

void ServerConfig::set_location( Location location ) {
	this->location.push_back( location );
}

std::vector< Location > ServerConfig::get_location() const {
	return ( location );
}

Location ServerConfig::get_location_idx( size_t idx ) const {
	if ( idx >= location.size() ) {
		throw ( std::string( "get_location_idx: out_of_range" ) );
	}
	return ( location[ idx ] );
}

std::vector< std::string > ServerConfig::get_server_names() const {
	return ( server_names );
}

std::string ServerConfig::get_server_names_idx( size_t idx ) const {
	if ( idx >= server_names.size() ) {
		throw ( std::string( "get_server_names_idx: out_of_range" ) );
	}
	return ( server_names[ idx ] );
}
std::vector< std::pair<int, std::string > > ServerConfig::get_error_pages() const {
	return ( error_pages );
}

std::pair<int, std::string> ServerConfig::get_error_pages_idx( size_t idx ) const {
	if ( idx >= error_pages.size() ) {
		throw ( std::string( "get_error_pages_idx: out_of_range" ) );
	}
	return ( error_pages[ idx ] );
}

void ServerConfig::print() {
	for ( size_t i = 0; i < server_names.size(); i++ ) {
		std::cout << "server_names: " << server_names.at( i ) << std::endl;
	}
	for ( size_t i = 0; i < error_pages.size(); i++ ) {
		std::cout << "error_pages: " << error_pages.at( i ).first << error_pages.at( i ).second << std::endl;
	}
	std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
	for ( size_t i = 0; i < location.size(); i++ ) {
		std::cout << "------------Location--BEG-------------\n";
		location.at( i ).print();
		std::cout << "------------Location--END-------------\n";
	}
}

void ServerConfig::clear() {
	server_names.clear();
	error_pages.clear();
	client_max_body_size = -1;
	location.clear();
}

void ServerConfig::fill_server_name( const std::vector< std::string > & directive ) {
	if ( directive.size() < 2 ) {
		throw ( std::string( "server_name size" ) );
	}
	for ( size_t i = 1; i < directive.size(); i++ ) {
		server_names.push_back( directive.at( i ) );	
	}
	for ( size_t i = 0; i < server_names.size(); i++ ) {
		for ( size_t j = i + 1; j < server_names.size(); j++ ) {
			if ( server_names.at( i ) == server_names.at( j ) )
				throw ( std::string( "server_name duplicated" ) );
		}
	}
}

void ServerConfig::fill_error_page( const std::vector< std::string > & directive ) {
	int status;
	std::stringstream ss;

	if ( directive.size() != 3 ) {
		throw ( std::string( "error_page size" ) );
	}
	ss << directive.at( 1 );
	ss >> status;
	if (ss.fail() || !ss.eof()) {
		throw ( std::string( "invalid error_page code" ) );
	}
	error_pages.push_back( std::make_pair( status, directive.at( 2 ) ) );
}

void ServerConfig::fill_client_max_body_size( const std::vector< std::string > & directive ) {
	int size;
	std::stringstream ss;

	if ( directive.size() != 2 ) {
		throw ( std::string( "client_max_body_size size" ) );
	}
	ss << directive.at( 1 );
	ss >> size;
	if (ss.fail() || !ss.eof()) {
		throw ( std::string( "invalid max_body_size" ) );
	}
	if ( size < 0 )
		throw ( std::string( "invalid max_body_size" ) );
	client_max_body_size = size;
}

void ServerConfig::fill( const std::vector< std::vector< std::string > > & directives ) {
	for ( size_t i = 0; i < directives.size(); i++ ) {
		if ( directives.at( i ).at( 0 ) == "server_name" ) {
			fill_server_name( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "error_page" ) {
			fill_error_page( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "client_max_body_size" ) {
			fill_client_max_body_size( directives.at( i ) );
		}
	}
}
