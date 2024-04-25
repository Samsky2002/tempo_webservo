#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerInfo.hpp"
# include "Parser.hpp"

class Server
{
	public:
		std::vector<ServerInfo> serverInfo;
		ServerInfo tmpServerInfo;
		ServerConfig tmpServerConfig;
		Location tmpLocation;
		Server();
		Server & operator=(const Server & server);
		~Server();
		ServerInfo getServer( int id ) const;
		void execute( Parser & parser );
		void setup( Parser & parser );
		void launch();
		void fill( const std::vector< ServerContext > & serverContext );
		void fill_server_info();
		void print();
		void clear();
};

#endif
