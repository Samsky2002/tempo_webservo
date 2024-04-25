#include "webserv.hpp"

// check if you are going to use exit or not
//

// to improve this split

// you need to figure out what you should do with new line
void Parser::parse_location() {
	tmpLocationContext.locationDirectives.push_back( directives.at( 0 ) );
	directives.erase( directives.begin() );
	while ( !directives.empty() ) {
		if ( directives.at( 0 ).empty() ) {
			directives.erase( directives.begin() );
			continue ;
		}
		if ( directives.at( 0 ).at( 0 ).size() > 1 && directives.at( 0 ).at( 0 ).at( 0 ) == '\t' && directives.at( 0 ).at( 0 ).at( 1 ) == '\t' ) {
			directives.at( 0 ).at( 0 ) = directives.at( 0 ).at( 0 ).substr( 2 );
			tmpLocationContext.locationDirectives.push_back( directives.at( 0 ) );
			directives.erase( directives.begin() );
		}
		else {
			break ;
		}
	}
}

void Parser::parse_server() {
	while ( !directives.empty() ) {
		if ( directives.at( 0 ).empty() ) {
			directives.erase( directives.begin() );
			continue ;
		}
		if ( directives.at( 0 ).at( 0 ).at( 0 ) == '\t' ) {
			directives.at( 0 ).at( 0 ) = directives.at( 0 ).at( 0 ).substr( 1 );
			if ( directives.at( 0 ).at( 0 ) == "location" ) {
				parse_location();
				tmpServerContext.locationContext.push_back( tmpLocationContext );
				tmpLocationContext.clear();
				continue ;
			}
			tmpServerContext.serverDirectives.push_back( directives.at( 0 ) );
			directives.erase( directives.begin() );
		}
		else {
			break ;
		}
	}
}

void Parser::fill() {
	while ( !directives.empty() ) {
		if ( directives.at( 0 ).size() == 1 && directives.at( 0 ).at( 0 ) == "server" ) {
			directives.erase( directives.begin() );
			parse_server();
			serverContext.push_back( tmpServerContext );
			tmpServerContext.clear();
			continue ;
		}
		else {
			throw ( std::string( "unknown syntax" ) );
		}
	}
}

void Parser::print() {
	for ( size_t i = 0; i < serverContext.size(); i++ ) {
		std::cout << "-------------------------------------ServerContext-Beg-----------------------------------\n";
		serverContext.at( i ).print();
		std::cout << "-----------------------------------------ServerContext-End--------------------------------\n";
	}
}

void Parser::check_server_keys( const std::vector< std::vector< std::string > > & serverDirectives ) {
	for ( size_t i = 0; i < serverDirectives.size(); i++ ) {
		if ( serverDirectives.at( i ).empty() )
			throw ( std::string( "syntax error" ) );
		//std::cout << serverDirectives.at( i ).at( 0 ) << std::endl;
		if ( serverDirectives.at( i ).at( 0 ) != "listen" && serverDirectives.at( i ).at( 0 ) != "server_name" && serverDirectives.at( i ).at( 0 ) != "error_page" && serverDirectives.at( i ).at( 0 ) != "client_max_body_size" ) {
			throw ( std::string( "unknown server key" ) );
		}
	}
}

void Parser::check_location_keys( const std::vector< std::vector< std::string > > & locationDirectives ) {
	for ( size_t i = 0; i < locationDirectives.size(); i++ ) {
		//std::cout << locationDirectives.at( i ).at( 0 ) << std::endl;
		if ( locationDirectives.at( i ).empty() )
			throw ( std::string( "syntax error" ) );
		if ( locationDirectives.at( i ).at( 0 ) != "allow" && locationDirectives.at( i ).at( 0 ) != "return" && locationDirectives.at( i ).at( 0 ) != "autoindex" && locationDirectives.at( i ).at( 0 ) != "root" && locationDirectives.at( i ).at( 0 ) != "index" && locationDirectives.at( i ).at( 0 ) != "upload" && locationDirectives.at( i ).at( 0 ) != "location" ) {
			throw ( std::string( "unknown location key" ) );
		}
	}
}

void Parser::check_duplicated_keys( const std::vector< std::vector< std::string > > & directives ) {
	for ( size_t i = 0; i < directives.size(); i++ ) {
		//std::cout << "i: " << directives.at( i ).at( 0 ) << std::endl;
		for ( size_t j = i + 1; j < directives.size(); j++ ) {
			//std::cout << "j: " << directives.at( j ).at( 0 ) << std::endl;
			if ( directives.at( i ).at( 0 ) == directives.at( j ).at( 0 ) && directives.at( i ).at( 0 ) != "error_page" && directives.at( i ).at( 0 ) != "location" )
				throw ( std::string( "duplicated keys" ) );
		}
	}
}
void Parser::check_necessary_server_keys( const std::vector< std::vector< std::string > > & serverDirectives ) {
	for ( size_t i = 0; i < serverDirectives.size(); i++ ) {
		if ( serverDirectives.at( i ).at( 0 ) == "listen" ) {
			return ;
		}
	}
	throw ( std::string( "server necessary to run not found" ) );
}

void Parser::check_necessary_location_keys( const std::vector< std::vector< std::string > > & locationDirectives ) {
	bool rootFound = false;
	bool returnFound = false;

	for ( size_t i = 0; i < locationDirectives.size(); i++ ) {
		if ( locationDirectives.at( i ).at( 0 ) == "root" ) {
			rootFound = true;
		}
		if ( locationDirectives.at( i ).at( 0 ) == "return" ) {
			returnFound = true;
		}
	}
	if ( ( rootFound && returnFound ) || (  !rootFound && !returnFound ) ) {
		throw ( std::string( "location necessary to run not found" ) );
	}
}

void Parser::check_server_directives( const std::vector< std::vector< std::string > > & serverDirectives ) {
	check_server_keys( serverDirectives );
	check_duplicated_keys( serverDirectives );
	check_necessary_server_keys( serverDirectives );
}

void Parser::check_location_directives( const std::vector< std::vector< std::string > > & locationDirectives ) {
	check_location_keys( locationDirectives );
	check_duplicated_keys( locationDirectives );
	check_necessary_location_keys( locationDirectives );

}

void Parser::check() {
	for ( size_t i = 0; i < serverContext.size(); i++ ) {
		check_server_directives( serverContext.at( i ).serverDirectives );
		if ( serverContext.at( i ).locationContext.empty() ) {
			throw ( std::string( "no location found" ) );
		}
		for ( size_t j = 0; j < serverContext.at( i ).locationContext.size(); j++ ) {
			check_location_directives( serverContext.at( i ).locationContext.at( j ).locationDirectives );
		}
	}
}

void Parser::execute( const std::string & file ) {
	std::ifstream infile( file );
	std::string line;

	if ( !infile.is_open() ) {
		std::cerr << "file not found\n";
		exit( 1 );
	}
	while ( getline( infile, line ) ) {
		directives.push_back( split( line, ' ' ) );
	}
	try {
		fill();
		check();
	}
	catch ( const std::string & e ) {
		std::cout << "error: " << e << std::endl;
		//std::cout << directives.at( 0 ).at(0) << std::endl;
		exit(0);
	}
}
