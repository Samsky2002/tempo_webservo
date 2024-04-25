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
		//this->client = http.client;
		//this->multiplex = http.multiplex;
		this->config_file = http.config_file;
	}
	return ( *this );
}

Http::~Http()
{

}

/*void Http::setup(const std::string & file)
{
	server.setup( file );	
	//multiplex.setup();
	// maybe setup the multiplex
}*/

void Http::execute( const std::string & file )
{
	parser.execute( file );
	server.execute( parser );
	multiplex.execute( server );
}

/*void Http::multiplexing()
{
	multiplex.multiplexing(this->client, this->config);	
}*/

// i need to know how the multiplexing will need listen so i can make it work
