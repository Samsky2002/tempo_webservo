#include "webserv.hpp"

int	main( int argc, char **argv )
{
	std::string file;
	Http http;

	signal(SIGPIPE, SIG_IGN);
	if ( argc > 2 )
		return ( 1 );
	file = argv[1] ? argv[1] : "config_file";
	try {
		http.execute( file );
	}
	catch ( std::string & e ) {
		std::cout << "main: " << e << std::endl;
	}
	catch ( std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	catch ( const char * e ) {
		if ( strcmp( e, "failed" ) == 0 ) {
			return ( 1 );
		}
		//std::cout << e.what() << std::endl;
	}
	catch ( ... ) {
		std::cout << "something went wrong here\n";
	}
}
