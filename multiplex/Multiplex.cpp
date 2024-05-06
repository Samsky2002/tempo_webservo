#include "webserv.hpp"

Multiplex::Multiplex()
{
	// Adding constructor
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;
}

Multiplex::Multiplex( const Multiplex & multiplex )
{
	*this = multiplex;
}

Multiplex & Multiplex::operator=( const Multiplex & multiplex )
{
	if ( this != &multiplex )
	{
		std::cout << "copy assinment multiplex";
		//No need to copy bcs Our Multiplexer should no copiable;
	}
	return ( *this );
}

Multiplex::~Multiplex()
{
}

void charPointerToVector(const char* charArray, std::vector<char>& charVector, ssize_t len) 
{
	// Assuming charArray is null-terminated
	ssize_t i = 0;
	while ( i < len ) {
		charVector.push_back( charArray[i] );
		i++;
	}
}

void Multiplex::execute( const Server & server )
{
	setup( server.get_serverInfo() );
	multiplexing( server );
}

void Multiplex::setup( const std::vector< ServerInfo > & serverInfo )
{
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&tmp_readfds);
	FD_ZERO(&tmp_writefds);
	for (size_t i = 0; i < serverInfo.size(); i++)
	{
		FD_SET(serverInfo[i].get_id(), &tmp_readfds);
	}
	nfds = serverInfo[ serverInfo.size() - 1].get_id();
}

bool Multiplex::isServer( const std::vector< ServerInfo >& serverInfo, int &socket )
{
	for ( size_t i = 0; i < serverInfo.size(); i++ )
	{
		if ( socket == serverInfo[i].get_id() )
		{
			return ( true );
		}
	}
	return ( false );
}

/////////////////////////////////////////////Helper Function///////////////////////////////////////////////

void	Multiplex::HandleNewConnection(int &ServerSocket, const Server &server)
{
	newfd = accept(ServerSocket, NULL, NULL);
	if (newfd == -1){
		perror("Error in Accepting new Client: ");
		return;
	}
	set_nonblocking(newfd);
	std::cout << "Accept :" << newfd << std::endl;
	FD_SET(newfd, &tmp_readfds);
	nfds = std::max(newfd, nfds);
	client.addClient(ClientInfo(newfd, server.get_serverInfo_idx(ServerSocket)));
}

void	Multiplex::HandleCgiRead(int &CgiSocket)
{
	client.fillCgiRead(CgiSocket, tmp_readfds);
}

void	Multiplex::HandleClientRead(int &clientSocket)
{
	char	recv_buffer[1024];
	std::vector<char > buffer;

	memset( recv_buffer, '\0', sizeof( recv_buffer ) );
	buffer.clear();
	int rec = recv(clientSocket, recv_buffer, sizeof(recv_buffer), 0);
	if (rec <= 0)
	{
		if (rec == 0){
			std::cout << "Connection Closed" << std::endl;
		}
		client.removeClient(clientSocket);
		FD_CLR(clientSocket, &tmp_readfds);
		FD_CLR(clientSocket, &tmp_writefds);
		close( clientSocket );
	}
	else{
		charPointerToVector(recv_buffer, buffer, rec);
		::print(buffer);
		try{
			client.requestSetup(buffer, clientSocket);
		}catch( int e){
			FD_SET(clientSocket, &tmp_writefds);
		}
	}
}

void	Multiplex::HandleCgiWrite(int &CgiSocket)
{
	client.fillCgiWrite(CgiSocket, tmp_writefds);
}

void	Multiplex::HandleClientWrite(int &clientSocket)
{
	std::string tmp;

	if ( !client.response_status( clientSocket ) ) {
		try {
			client.responseSetup( clientSocket, tmp_readfds, tmp_writefds, nfds );
		}
		catch ( int e ) {
			result = client.result( clientSocket );
		}
	}
	else {
		try {
			if ( result.empty() ) {
				throw ( 1 );
			}
			tmp = result.substr( 0, 2000000 );
			std::cout << "Enter here\n";
			int sen = send( clientSocket, tmp.c_str(), tmp.length(), 0 );

			if ( sen <= 0) {
				result.erase(0, sen);
				perror( "send error: " );
				throw ( 1 );
			}
			result.erase(0, sen);
		}
		catch ( int e ) {
			client.clear(clientSocket);
			std::cout << "Clearing write\n";
			FD_CLR(clientSocket, &tmp_writefds);
		}
	}
}

void	Multiplex::multiplexing( const Server& server)
{
	while (true)
	{
		readfds = tmp_readfds;
		writefds = tmp_writefds;
		if (select(nfds + 1, &readfds, &writefds, NULL, NULL) == -1)
		{
			perror("select");
		}
		for (int i = 0; i <= nfds; i++)
		{
			if (FD_ISSET(i, &readfds))
			{
				// std::cout << "***********************READ HANLDER *******************************" << std::endl;
				if (isServer(server.get_serverInfo(), i) ){
					HandleNewConnection(i , server);
				}
				else if ( client.isCgiRead( i ) ){
					HandleCgiRead( i );
				}
				else{
					HandleClientRead( i );
				}
			}
			else if (FD_ISSET( i, &writefds )) {
				// std::cout << "***********************WRITE HANLDER ******************************" << std::endl;
				if (client.isCgiWrite(i)){
					HandleCgiWrite( i );
				}
				else {
					HandleClientWrite( i );
				}
			}
		}
	}
}
