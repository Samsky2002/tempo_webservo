#include "Http.hpp"

Http::Http()
{

}

Http::Http( const Http & http )
{
	*this = http;
}

Http & Http::operator=( const Http & http )
{
	if ( this != &http )
	{
		this->server = http.server;
		this->parser = http.parser;
		// this->multiplex = http.multiplex;
		this->config_file = http.config_file;
	}
	return ( *this );
}

Http::~Http()
{

}

void Http::execute( const std::string & file )
{
	parser.execute( file );
	server.execute( parser );
	multiplex.execute( server );
}
