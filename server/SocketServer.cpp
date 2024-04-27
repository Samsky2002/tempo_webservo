#include "webserv.hpp"

SocketServer::SocketServer() {}

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
	this->backlog = 20;
}

SocketServer::SocketServer( const SocketServer & socketServer )
{
	*this = socketServer;
}

SocketServer & SocketServer::operator=( const SocketServer & socketServer )
{
	if ( this != &socketServer )
	{
		this->port = socketServer.port;
		this->host = socketServer.host;
		this->family = socketServer.family;
		this->addrLen = socketServer.addrLen;
		this->serverAddr = socketServer.serverAddr; // tempo
		this->domain = socketServer.domain;
		this->type = SOCK_STREAM;
		this->protocol = 0;
		this->level = SOL_SOCKET;
		this->option_name = SO_REUSEADDR;
		this->option_value = 1;
		this->option_len = sizeof( int );
		this->backlog = 20;
	}
	return ( *this );
}

SocketServer::~SocketServer()
{

}

void	guard(int returnValue, int listner, std::string msg)
{
	if (returnValue == -1)
	{
		std::cerr << msg << strerror(errno) << std::endl;
		close(listner);
		throw(1);
	}
}


void SocketServer::launch( int & id )
{
	try{
				//create socket;
		listener = socket(domain, type, protocol);
		guard(listener, listener,  "Faillue on creating Socket ");

				//customizing socket ;
		guard(setsockopt(listener, level, option_name, &option_value, option_len), listener, "Faillue on customizing  Socket " );

		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &option_value, option_len);
		setsockopt(listener, SOL_SOCKET, SO_REUSEPORT, &option_value, option_len);

		// int current_flag = fcntl(listener, F_GETFL);
		// if (current_flag == -1)
		// {
		// 	std::cout << " error in fcntl get flag "<< std::endl;
		// 	exit(1);
		// }
		// if (fcntl(listener , F_SETFL, current_flag | O_NONBLOCK) == -1)
		// {
		// 	perror ("error in setting non_block flag ");
		// 	exit(1);
		// }

				//biding socket ;
		guard(bind(listener, (const sockaddr *) &serverAddr, addrLen), listener, "Faillue on binding Socket " );

				//listening to upcoming conection
		guard(listen(listener, backlog), listener,  "Faillue on Listening " );

		std::cout << "We got the server " << listener << "that have the adreess " << this->host 
		<< " and the Port " << this->port << std::endl;

	id = this->listener;
	}catch(int number)
	{
		close(listener);
		std::cout << "Stoping Server ..." << std::endl;
		
	}
	/* hadi kan savi biha socket for the client as an id*/
}

