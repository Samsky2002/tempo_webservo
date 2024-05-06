#include "webserv.hpp"

void ServerContext::clear() {
	locationContext.clear();
	serverDirectives.clear();
}

ServerContext::ServerContext() {

}

ServerContext::ServerContext( const ServerContext & serverContext ) {
	*this = serverContext;
}

ServerContext & ServerContext::operator=( const ServerContext & serverContext ) {
	if ( this != &serverContext ) {
		this->locationContext = serverContext.locationContext;
		this->serverDirectives = serverContext.serverDirectives;
	}
	return ( *this );
}

ServerContext::~ServerContext() {

}

void ServerContext::print() {
	std::cout << "----------------------serverDirectives-Beg---------------------\n";
	for ( size_t i = 0; i < serverDirectives.size(); i++ ) {
		for ( size_t j = 0; j < serverDirectives.at( i ).size(); j++ ) {
			std::cout << serverDirectives.at( i ).at( j ) << "|";
		}
		std::cout << std::endl;
	}
	std::cout << "----------------------serverDirectives-End---------------------\n";
	for ( size_t i = 0; i < locationContext.size(); i++ ) {
		std::cout << "----------------------locationContext-Beg---------------------\n";
		locationContext.at( i ).print();
		std::cout << "----------------------locationContext-End---------------------\n";
	}
}

std::vector< LocationContext > ServerContext::get_locationContext() const {
	return ( locationContext );
}

std::vector< std::vector< std::string > > ServerContext::get_serverDirectives() const {
	return ( serverDirectives );
}

void ServerContext::set_locationContext( LocationContext locationContext ) {
	this->locationContext.push_back( locationContext );
}

void ServerContext::set_serverDirectives( std::vector< std::string > serverDirective ) {
	this->serverDirectives.push_back( serverDirective );
}
