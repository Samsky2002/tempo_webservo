#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerInfo.hpp"
# include "Parser.hpp"

class Server
{
	private:
		std::vector<ServerInfo> serverInfo;
		ServerInfo tmpServerInfo;
		ServerConfig tmpServerConfig;
		Location tmpLocation;
	public:
		Server();
		Server(const Server & server);
		Server & operator=(const Server & server);
		~Server();
		ServerInfo get_serverInfo_idx( int id ) const;
		std::vector< ServerInfo > get_serverInfo() const;
		void execute( Parser & parser );
		void setup( Parser & parser );
		void launch();
		void fill_location( std::vector< LocationContext > locationContext );
		void fill( const std::vector< ServerContext > & serverContext );
		void fill_server_info();
		void print();
		void clear();
};

#endif
