#include "webserv.hpp"
SocketServer::SocketServer()
{

}

SocketServer::SocketServer( int port, std::string host )
{
	this->port = port;
	this->host = host;
	this->family = AF_INET;
	this->addrLen = sizeof( struct sockaddr );
	memset(&serverAddr, '\0', sizeof( struct sockaddr ));
	this->serverAddr.sin_family = this->family;
	this->serverAddr.sin_port = htons( port );
	this->serverAddr.sin_addr.s_addr = INADDR_ANY; // tempo
	this->domain = PF_INET;
	this->type = SOCK_STREAM;
	this->protocol = 0;
	this->level = SOL_SOCKET;
	this->option_name = SO_REUSEADDR;
	this->option_value = 1;
	this->option_len = sizeof( int );
	this->backlog = 100;

}

SocketServer::SocketServer( const SocketServer & socketServer )
{
	*this = socketServer;
}

SocketServer & SocketServer::operator=( const SocketServer & socketServer )
{
	if ( this != &socketServer )
	{
		/*this->port = socketServer.port;
		this->host = socketServer.host;
		this->family = socketServer.family;
		this->addrLen = socketServer.addrLen;
		this->serverAddr = socketServer->serverAddr; // tempo
		this->domain = ;
		this->type = SOCK_STREAM;
		this->protocol = 0;
		this->level = SOL_SOCKET;
		this->option_name = SO_REUSEADDR;
		this->option_value = 1;
		this->option_len = sizeof( int );
		this->backlog = 20;*/
	}
	return ( *this );
}

SocketServer::~SocketServer()
{

}

void SocketServer::launch( int & id )
{
	this->listener = socket( this->domain, this->type, this->protocol );
	if ( this->listener == -1 )
	{
		std::cerr << " socket error\n";
		return ;
	}
	if ( setsockopt( this->listener, this->level, this->option_name, &this->option_value, this->option_len ) == -1 )
	{
		std::cerr << " setsockopt error\n";
		return ;
	}
	if ( bind( this->listener, ( struct sockaddr * )&this->serverAddr, this->addrLen ) == -1)
	{
		std::cerr << " bind error\n";
		return ;
	}
	if ( listen( this->listener, this->backlog ) == -1)
	{
		std::cerr << " listen error\n";
		return ;
	}
	//FD_SET( this->listener, &multiplex.tmp_readfds );
	//multiplex.nfds = this->listener;
	id = this->listener;
	set_nonblocking( id );
	std::cout << "listening ... on port " << this->port << std::endl;
}
