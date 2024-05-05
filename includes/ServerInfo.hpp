#ifndef SERVERINFO_HPP
# define SERVERINFO_HPP
# include "ServerConfig.hpp"
# include "Parser.hpp"

class ServerInfo
{
	private:
		int id;
		int port;
		std::string host;
		std::vector< ServerConfig > serverConfig;
	public:
		ServerInfo();
		ServerInfo( const ServerInfo & serverInfo );
		ServerInfo & operator=( const ServerInfo & serverInfo );
		~ServerInfo();
		int get_id() const;
		int get_port() const;
		std::string get_host() const;
		void set_serverConfig( ServerConfig serverConfig );
		ServerConfig get_serverConfig_idx( size_t idx ) const;
		std::vector< ServerConfig > get_serverConfig() const;
		void launch();
		void print();
		void clear();
		void fill( const std::vector< std::vector< std::string > > & directives );
		void fill_host_port( const std::vector< std::string > & directive );
		void check_host( const std::string & strHost );
		void check_port( const std::string & strPort );
};

#endif
