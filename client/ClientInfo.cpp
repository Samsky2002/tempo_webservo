#include "webserv.hpp"

ClientInfo::ClientInfo()
{
}

ClientInfo::ClientInfo( int id, const ServerInfo & serverInfo )
{
	this->id = id;
	this->serverInfo = serverInfo;
}

ClientInfo::ClientInfo(const ClientInfo & clientInfo)
{
	*this = clientInfo;
}

ClientInfo & ClientInfo::operator=(const ClientInfo & clientInfo)
{
	if ( this != &clientInfo )
	{
		//std::cout << "copy assignment clientInfo\n";
		this->id = clientInfo.id;
		this->request = clientInfo.request;
		this->response = clientInfo.response;
		this->serverInfo = clientInfo.serverInfo;
	}
	return ( *this );
}

ClientInfo::~ClientInfo()
{

}

// this isn't the best approach
void ClientInfo::requestSetup( std::vector<char> & buffer )
{
	try {
		request.setup( buffer );
	}
	catch ( int e )
	{
		buffer.clear();
		throw ( 1 );
	}
}

// BE CAREFUL : std::to_string C++11
void ClientInfo::responseSetup()
{
	try {
		response.setup( request, serverInfo );
	}
	catch ( int e ) {
		request.clear();
		throw ( 1 );
	}
}

std::string ClientInfo::result()
{
	return ( response.result() );
}
// do a request and response setup for the naming
// need to clear everything
