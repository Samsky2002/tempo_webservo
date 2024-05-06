#ifndef PARSER_HPP
# define PARSER_HPP

# include "ServerContext.hpp"

class Parser {
	private:
		std::vector< ServerContext > serverContext;
		ServerContext tmpServerContext;
		LocationContext tmpLocationContext;
		std::vector< std::vector< std::string > > directives;
	public:
		Parser();
		Parser( const Parser & parser );
		Parser & operator=( const Parser & parser );
		~Parser();
		std::vector< ServerContext > get_serverContext() const;
		void execute( const std::string & file );
		void fill();
		void parse_server();
		void parse_location();
		void print();
		void check();
		void check_server_directives( const std::vector< std::vector< std::string > > & serverDirectives );
		void check_location_directives( const std::vector< std::vector< std::string > > & locationDirectives );
		void check_server_keys( const std::vector< std::vector< std::string > > & serverDirectives );
		void check_location_keys( const std::vector< std::vector< std::string > > & locationDirectives );
		void check_duplicated_keys( const std::vector< std::vector< std::string > > & directives );
		void check_necessary_server_keys( const std::vector< std::vector< std::string > > & serverDirectives );
		void check_necessary_location_keys( const std::vector< std::vector< std::string > > & locationDirectives );
};

#endif
