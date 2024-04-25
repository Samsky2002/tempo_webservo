#include "webserv.hpp"

void ServerContext::clear() {
	locationContext.clear();
	serverDirectives.clear();
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
