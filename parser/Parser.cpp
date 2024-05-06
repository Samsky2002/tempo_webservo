#include "webserv.hpp"

Parser::Parser() {

}

Parser::Parser( const Parser & parser ) {
	*this = parser;
}

Parser & Parser::operator=( const Parser & parser ) {
	if ( this != &parser ) {
		this->serverContext = parser.serverContext;
		this->tmpServerContext = parser.tmpServerContext;
		this->tmpLocationContext = parser.tmpLocationContext;
		this->directives = parser.directives;
	}
	return ( *this );
}

Parser::~Parser() {

}

std::vector< ServerContext > Parser::get_serverContext() const {
	return ( serverContext );
}

void Parser::parse_location() {
	tmpLocationContext.set_locationDirectives( directives.at( 0 ) );
	directives.erase( directives.begin() );
	while ( !directives.empty() ) {
		if ( directives.at( 0 ).empty() ) {
			directives.erase( directives.begin() );
			continue ;
		}
		if ( directives.at( 0 ).at( 0 ).size() > 1 && directives.at( 0 ).at( 0 ).at( 0 ) == '\t' && directives.at( 0 ).at( 0 ).at( 1 ) == '\t' ) {
			directives.at( 0 ).at( 0 ) = directives.at( 0 ).at( 0 ).substr( 2 );
			tmpLocationContext.set_locationDirectives( directives.at( 0 ) );
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
				tmpServerContext.set_locationContext( tmpLocationContext );
				tmpLocationContext.clear();
				continue ;
			}
			tmpServerContext.set_serverDirectives( directives.at( 0 ) );
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
		if ( serverDirectives.at( i ).at( 0 ) != "listen" && serverDirectives.at( i ).at( 0 ) != "server_name" && serverDirectives.at( i ).at( 0 ) != "error_page" && serverDirectives.at( i ).at( 0 ) != "client_max_body_size" ) {
			throw ( std::string( "unknown server key" ) );
		}
	}
}

void Parser::check_location_keys( const std::vector< std::vector< std::string > > & locationDirectives ) {
	for ( size_t i = 0; i < locationDirectives.size(); i++ ) {
		if ( locationDirectives.at( i ).empty() )
			throw ( std::string( "syntax error" ) );
		if ( locationDirectives.at( i ).at( 0 ) != "allow" && locationDirectives.at( i ).at( 0 ) != "return" && locationDirectives.at( i ).at( 0 ) != "autoindex" && locationDirectives.at( i ).at( 0 ) != "root" && locationDirectives.at( i ).at( 0 ) != "index" && locationDirectives.at( i ).at( 0 ) != "upload" && locationDirectives.at( i ).at( 0 ) != "location" ) {
			throw ( std::string( "unknown location key" ) );
		}
	}
}

void Parser::check_duplicated_keys( const std::vector< std::vector< std::string > > & directives ) {
	for ( size_t i = 0; i < directives.size(); i++ ) {
		for ( size_t j = i + 1; j < directives.size(); j++ ) {
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
		check_server_directives( serverContext.at( i ).get_serverDirectives() );
		if ( serverContext[ i ].get_locationContext().empty() ) {
			throw ( std::string( "no location found" ) );
		}
		for ( size_t j = 0; j < serverContext.at( i ).get_locationContext().size(); j++ ) {
			check_location_directives( serverContext.at( i ).get_locationContext().at( j ).get_locationDirectives() );
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
