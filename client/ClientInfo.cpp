#include "webserv.hpp"

ClientInfo::ClientInfo()
{
	id = -1;
	response_status = false;
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
		this->id = clientInfo.id;
		this->request = clientInfo.request;
		this->response = clientInfo.response;
		this->serverInfo = clientInfo.serverInfo;
		this->response_status = clientInfo.response_status;
	}
	return ( *this );
}

ClientInfo::~ClientInfo()
{
}

int ClientInfo::get_id() const {
	return ( id );
}

Response & ClientInfo::get_response() {
	return ( response );
}

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
void ClientInfo::responseSetup( fd_set & tmp_readfds, fd_set & tmp_writefds, int & nfds )
{
	try {
		response.setup( request, serverInfo, tmp_readfds, tmp_writefds, nfds );
	}
	catch ( int e ) {
		if ( e == 1 ) {
			request.clear();
			response_status = true;
			throw ( 1 );
		}
	}
}
// need to clear everything

std::string ClientInfo::result()
{
	return ( response.result() );
}

bool ClientInfo::get_response_status() const {
	return ( response_status );
}

void ClientInfo::clear() {
    std::cout << "clearing\n";
    request.clear();
    response.clear();
    response_status = false;
    // start time and stuff maybe not
}