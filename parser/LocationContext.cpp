#include "webserv.hpp"


LocationContext::LocationContext() {

}

LocationContext::LocationContext( const LocationContext & locationContext ) {
	*this = locationContext;
}

LocationContext & LocationContext::operator=( const LocationContext & locationContext ) {
	if ( this != &locationContext ) {
		this->locationDirectives = locationContext.locationDirectives;
	}
	return ( *this );
}

LocationContext::~LocationContext() {

}

void LocationContext::clear() {
	locationDirectives.clear();
}

void LocationContext::print() {
	for ( size_t i = 0; i < locationDirectives.size(); i++ ) {
		for ( size_t j = 0; j < locationDirectives.at( i ).size(); j++ ) {
			std::cout << locationDirectives.at( i ).at( j ) << "|";
		}
		std::cout << std::endl;
	}
}
std::vector< std::vector< std::string > > LocationContext::get_locationDirectives() const {
	return ( locationDirectives );
}

void LocationContext::set_locationDirectives( std::vector< std::string > directive ) {
	locationDirectives.push_back( directive );
}
