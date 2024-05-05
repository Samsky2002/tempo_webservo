#ifndef SERVERCONTEXT_HPP
# define SERVERCONTEXT_HPP

# include "LocationContext.hpp"

class ServerContext {
	private:
		std::vector< LocationContext > locationContext;
		std::vector< std::vector< std::string > > serverDirectives;
	public:
		ServerContext();
		ServerContext( const ServerContext & serverContext );
		ServerContext & operator=( const ServerContext & serverContext );
		~ServerContext();
		std::vector< LocationContext > get_locationContext() const;
		std::vector< std::vector< std::string > > get_serverDirectives() const;
		void set_locationContext( LocationContext locationContext );
		void set_serverDirectives( std::vector< std::string > serverDirective );
		void clear();
		void print();
};

#endif
