#include "webserv.hpp"

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
