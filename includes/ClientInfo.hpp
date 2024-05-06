#ifndef CLIENTINFO_HPP
# define CLIENTINFO_HPP
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConfig.hpp"

class ClientInfo
{
	private:
		int id;
		Request request;
		Response response;
		ServerInfo serverInfo;
		bool response_status;
	public:
		ClientInfo();
		ClientInfo( int id, const ServerInfo & serverInfo );
		ClientInfo( const ClientInfo & clientInfo);
		ClientInfo & operator=( const ClientInfo & clientInfo);
		~ClientInfo();
		int get_id() const;
		Response & get_response();
		void setup( std::vector<char> & buffer );
		void requestSetup( std::vector<char> & buffer );
		void responseSetup( fd_set & tmp_readfds, fd_set & tmp_writefds, int & nfds );
		std::string result();
		bool get_response_status() const;
		void	clear();
};

#endif
