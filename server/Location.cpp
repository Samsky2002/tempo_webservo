#include "webserv.hpp"

Location::Location()
{
	autoindex = false;
}

Location::Location(const Location & location)
{
	*this = location;
}

Location & Location::operator=(const Location & location)
{
	if ( this != &location )
	{
		this->path = location.path;
		this->methods = location.methods;
		this->redirection = location.redirection;
		this->root = location.root;
		this->autoindex = location.autoindex;
		this->index = location.index;
		this->upload = location.upload;
	}
	return (*this);
}

Location::~Location()
{

}

std::string Location::get_path() const {
	return ( path );
}

std::string Location::get_root() const {
	return ( root );
}

std::string Location::get_redirection() const {
	return ( redirection );
}

std::vector< std::string > Location::get_index() const {
	return ( index );
}

std::string Location::get_index_idx( size_t idx ) const {
	if ( idx >= index.size() ) {
		throw ( std::string( "get_index_idx: out_of_range" ) );
	}
	return ( index[ idx ] );
}

bool Location::get_autoindex() const {
	return ( autoindex );
}

std::vector< std::string > Location::get_methods() const {
	return ( methods );
}

std::string Location::get_methods_idx( size_t idx ) const {
	if ( idx >= methods.size() ) {
		throw ( std::string( "get_methods_idx: out_of_range" ) );
	}
	return ( methods[ idx ] );
}

std::string Location::get_upload() const {
	return ( upload );
}

void Location::print() {
	std::cout << "path: " << path << std::endl;
	for ( size_t i = 0; i < methods.size(); i++ ) {
		std::cout << "methods: " << methods.at( i ) << std::endl;
	}
	std::cout << "redirection: " << redirection << std::endl;
	std::cout << "root: " << root << std::endl;
	std::cout << "autoindex: " << autoindex << std::endl;
	for ( size_t i = 0; i < index.size(); i++ ) {
		std::cout << "index: " << index.at( i ) << std::endl;
	}
	std::cout << "upload: " << upload << std::endl;
}

void Location::clear() {
	path.clear();
	methods.clear();
	redirection.clear();
	root.clear();
	autoindex = false;
	index.clear();
	upload.clear();
}

void Location::fill_location_path( const std::vector< std::string > & directive ) {
	if ( directive.size() != 2 ) {
		throw ( std::string( "location path size" ) );
	}
	path = directive.at( 1 );
}
void Location::fill_methods( const std::vector< std::string > & directive ) {
	if ( directive.size() < 2 || directive.size() > 4 ) {
		throw ( std::string( "methdos size" ) );
	}
	for ( size_t i = 1; i < directive.size(); i++ ) {
		if ( directive.at( i ) != "GET" && directive.at( i ) != "POST" && directive.at( i ) != "DELETE" ) {
			throw ( std::string( "unknown method" ) );
		}
		methods.push_back( directive.at( i ) );
	}
	for ( size_t i = 0; i < methods.size(); i++ ) {
		for ( size_t j = i + 1; j < methods.size(); j++ ) {
			if ( methods.at( i ) == methods.at( j ) )
				throw ( std::string( "method duplicated" ) );
		}
	}
}

void Location::fill_redirection( const std::vector< std::string > & directive ) {
	if ( directive.size() != 2 ) {
		throw ( std::string( "return size" ) );
	}
	redirection = directive.at( 1 );
}
void Location::fill_root( const std::vector< std::string > & directive ) {
	if ( directive.size() != 2 ) {
		throw ( std::string( "root size" ) );
	}
	root = directive.at( 1 );
}

void Location::fill_autoindex( const std::vector< std::string > & directive ) {
	if ( directive.size() != 2 ) {
		throw ( std::string( "root size" ) );
	}
	if ( directive.at( 1 ) == "on" ) {
		autoindex = true;
	}
	else if ( directive.at( 1 ) == "off" ) {
		autoindex = false;
	}
	else {
		throw ( std::string( "unknown autoindex value" ) );
	}
}

void Location::fill_index( const std::vector< std::string > & directive ) {
	if ( directive.size() < 2 ) {
		throw ( std::string( "index size" ) );
	}
	for ( size_t i = 1; i < directive.size(); i++ ) {
		index.push_back( directive.at( i ) );
	}
	for ( size_t i = 0; i < index.size(); i++ ) {
		for ( size_t j = i + 1; j < index.size(); j++ ) {
			if ( index.at( i ) == index.at( j ) )
				throw ( std::string( "index duplicated" ) );
		}
	}
}

void Location::fill_upload( const std::vector< std::string > & directive ) {
	if ( directive.size() != 2 ) {
		throw ( std::string( "upload size" ) );
	}
	upload = directive.at( 1 );
}

void Location::check_trailing_slash() {
	if ( root.empty() || path.empty() )
		return ;
	if ( path.at( path.length() - 1 ) == '/' && root.at( root.length() - 1 ) != '/' ) {
		throw ( std::string( "not same trailing slash" ) );
	}
	else if ( path.at( path.length() - 1 ) != '/' && root.at( root.length() - 1 ) == '/' ) {
		throw ( std::string( "not same trailing slash" ) );
	}
}


void Location::fill( const std::vector< std::vector< std::string > > & directives ) {
	for ( size_t i = 0; i < directives.size(); i++ ) {
		if ( directives.at( i ).at( 0 ) == "location" ) {
			fill_location_path( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "allow" ) {
			fill_methods( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "return" ) {
			fill_redirection( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "root" ) {
			fill_root( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "autoindex" ) {
			fill_autoindex( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "index" ) {
			fill_index( directives.at( i ) );
		}
		else if ( directives.at( i ).at( 0 ) == "upload" ) {
			fill_upload( directives.at( i ) );
		}
	}
	check_trailing_slash();
	// checki path and root if same trailing slash
}
