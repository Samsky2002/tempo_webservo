#ifndef HTTP_HPP
# define HTTP_HPP
# include <vector>
# include "Server.hpp"
# include "Multiplex.hpp"
# include "Parser.hpp"

class Http
{
	private:
		Server server;
		Multiplex multiplex;
		Parser parser;
		std::string config_file;
	public:
		Http();
		Http( const Http & http );
		Http & operator=( const Http & http );
		~Http();
		void execute(const std::string & file);
};

#endif
