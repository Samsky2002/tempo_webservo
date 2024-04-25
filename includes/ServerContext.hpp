#ifndef SERVERCONTEXT_HPP
# define SERVERCONTEXT_HPP

# include "LocationContext.hpp"

class ServerContext {
	public:
		std::vector< LocationContext > locationContext;
		std::vector< std::vector< std::string > > serverDirectives;
		void clear();
		void print();
};

#endif


