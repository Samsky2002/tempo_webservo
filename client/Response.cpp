#include "webserv.hpp"

Response::Response()
{
	cgiExist = false;
}
Response::Response( const Response & response)
{
	*this = response;
}

Response & Response::operator=( const Response & response)
{
	if ( this != &response )
	{
		//std::cout << "copy assignment response\n";
		this->protocol = response.protocol;
		this->statusCode = response.statusCode;
		this->status = response.status;
		this->header = response.header;
		this->body = response.body;
		this->finalBody = response.finalBody;
		this->serverConfig = response.serverConfig;
		this->location = response.location;
		this->resource = response.resource;
		this->resourceType = response.resourceType;
	}
	return ( *this );
}

Response::~Response()
{

}

void Response::resourceHandle()
{
	struct stat path_stat;
	int result;

	if ( stat( resource.c_str(), &path_stat ) != 0)
	{
		std::cout << resource << std::endl;
		perror( "stat: " );
		throw ( 404 );
	}
	result = S_ISDIR( path_stat.st_mode );
    if (result == 1) {
		resourceType = Directory;
    } else if (result == 0) {
		resourceType = File;
    }
}

// to check this, understand it better
void Response::fillBody()
{
	std::stringstream ss;
	std::ifstream inputFile( resource, std::ios::binary );
	std::string line;
	//std::string test;

	if ( inputFile.is_open( ) ) {
		ss << inputFile.rdbuf();
		line = ss.str();
		body = std::vector<int>(line.begin(), line.end());
		inputFile.close();
		//test = std::string ( body.begin(), body.end() );
		//std::cout << test << std::endl;
	}
	else {
		std::cerr << "couldn't open file\n";
		throw ( 404 );
	}
}

bool Response::is_url_found_in_path( const std::string & requestPath, const std::string & locationPath )
{
	std::size_t found;

	if ( locationPath.length() > requestPath.length() ) {
		return ( false );
	}
	found = locationPath.find( requestPath.substr( 0, locationPath.length() ) );
	if ( found != std::string::npos ) {
		return ( true );
	}
	return ( false );
}

void Response::urlHandle( std::string requestPath)
{
	bool found;

	for ( size_t i = 0; i < serverConfig.location.size(); i++ ) {
		found = is_url_found_in_path( requestPath, serverConfig.location[i].path );
		if ( found ) {
			if ( !location.path.empty() ) {
				if ( location.path.length() < serverConfig.location[i].path.length() )
					location = serverConfig.location[i];
			}
			else
				location = serverConfig.location[i];
		}
	}
	if ( location.path.empty() )
	{
		std::cerr << "urlHandle\n";
		throw ( 404 );
	}
	resource = requestPath.replace( 0, location.path.length(), location.root );
}

void Response::redirectionHandle()
{
	if ( location.redirection.empty() )
		return ;
	header.push_back( std::make_pair( "Location", location.redirection ) );
	throw ( 301 );
}

void Response::methodHandle( const Request & request )
{
	if ( request.get_method() != "GET" && request.get_method() != "POST" && request.get_method() != "DELETE" )
		throw ( 405 );
	if ( location.methods.empty() )
		return ;
	for ( size_t i = 0; i < location.methods.size(); i++ ) {
		if ( request.get_method() == location.methods[i] )
			return ;
	}
	throw ( 405 );
}

void Response::setServerConfig( const Request & request, const ServerInfo & serverInfo )
{

	(void)serverInfo;
	(void)request;
	// this is just a temp solution
	/*std::string host;
	for ( size_t i = 0; i < request.header.size(); i++ )
	{
		if ( request.header[i].first == "Host" )
			host = request.header[i].second;
	}
	host = host.substr( 0, host.find(":") );
	if ( serverInfo.host == host )
	{
		serverConfig = serverInfo.serverConfig[0];	
		return ;
	}
	for ( size_t i = 0; i < serverInfo.serverConfig.size(); i++ )
	{
		if ( serverInfo.serverConfig[ i ].server_name == host )
		{
			serverConfig = serverInfo.serverConfig[ i ];
			return ;
		}
	}
	serverConfig = serverInfo.serverConfig[ 0 ];*/
}

bool Response::hasIndexHtml()
{
	std::string tmp;

	tmp = resource + "index.html";
	if ( !access( tmp.c_str(), F_OK ) )
	{
		resource = tmp;
		return ( true );
	}
	return ( false );
}

bool Response::hasIndex()
{
	std::string tmp;

	if ( location.index.empty() )
	{
		if ( hasIndexHtml() )
			return ( true );
		return ( false );
	}
	for ( size_t i = 0; i < location.index.size(); i++ )
	{
		tmp = resource;
		tmp += location.index[i];
		if ( !access( tmp.c_str(), F_OK ) )
		{
			//std::cout << "index: " << location.index[i] << std::endl;
			resource = tmp;
			return ( true );
		}
	}
	return ( false );
}

bool Response::hasTrailingSlach()
{
	if ( resource.at( resource.length() - 1 ) == '/' )
		return ( true );
	return ( false );
}

void Response::autoIndex( const Request & request )
{
	DIR * directory = opendir( resource.c_str() );
	
	finalBody += "<html><head><title>Index of " + request.get_path() + "</title></head>";
	finalBody += "<body><h1>Index of /</h1><hr><pre>";
	if ( directory != NULL ) {
		struct dirent * entry;
		while ( ( entry = readdir( directory ) ) != NULL ) {
			if ( entry->d_type == DT_REG )
				finalBody += "<a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a>\r\n";
			else if ( entry->d_type == DT_DIR )
				finalBody += "<a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "/</a>\r\n";
		}
		closedir( directory );
	}
	else {
		perror( "directory" );
	}
	finalBody += "</pre><hr></body></html>";
	header.push_back( std::make_pair( "Content-Type", "text/html" ) );
	throw ( 200 );
}
// maybe set the extension before hand

void Response::directoryHandle( const Request & request )
{
	if ( resourceType != Directory )
		return ;
	std::cout << resource << std::endl;
	if ( !hasTrailingSlach() )
	{
		header.push_back( std::make_pair( "Location", request.get_path() + "/" ) );
		throw ( 301 );
	}
	if ( hasIndex() )
		return ;
	if ( location.autoindex )
		autoIndex( request );
	throw ( 403 );
}

bool Response::isCgi()
{
	if ( location.path == "/cgi-bin" || location.path == "/cgi-bin/" )
	{
		cgiExist = true;
		return ( true );
	}
	return ( false );
}

void Response::cgiHandle( const Request & request )
{
	if ( !isCgi() )
		return ;
	cgi.setup( request, *this );
	cgi.launch();
	finalBody = cgi.body;
	cgi.clear();
	throw( 200 );
	// handle cgi
}

void Response::fileHandle( const Request & request )
{
	if ( request.get_method() == "POST" || request.get_method() == "DELETE")
		throw ( 405 );
	fillBody();
	finalBody = std::string( body.begin(), body.end() );
	header.push_back( std::make_pair( "Content-Type", getContentType() ) );
}

void Response::setStatus()
{
	if ( statusCode == 200 )
		status = "OK";
	else if ( statusCode == 404 )
		status = "Not Found";
	else if ( statusCode == 301 )
		status = "Moved Permanently";
	else if ( statusCode == 405 )
		status = "Not Allowed";
	else if ( statusCode == 403 )
		status = "Forbidden";
	else if ( statusCode == 400 )
		status = "Bad Request";
}

std::string Response::getExtension()
{
	size_t pos;
	std::string extension;

	pos = resource.find_last_of( '.' );
	if ( pos != std::string::npos )
	{
		extension = resource.substr( pos + 1 );
	}
	return ( extension );
}

std::string Response::getContentType()
{
	std::string extension = getExtension();

	if ( extension == "html" )
		return ( "text/html" );
	return ( "text/plain" );
}

void Response::setHeaders()
{
	if ( cgiExist )
		return ;
	//this header below i did it in every case no need to uncomment it
	//header.push_back( std::make_pair( "Content-Type", getContentType() ) );
	header.push_back( std::make_pair( "Content-Length", std::to_string( finalBody.length() ) ) );
}

void Response::fillDefaultErrorPage()
{
	finalBody = "<html>\n<head>\n\t<title>" + std::to_string( statusCode ) + " " + status + "</title>\n</head>\n";
	finalBody += "<body>\n<center>\n\t<h1>" + std::to_string( statusCode ) + " " + status + "</h1>\n</center>\n";
	finalBody += "<hr>\n<center>webserv/1.1</center>\n</body>\n</html>\n";
}

void Response::errorResponse()
{
	bool found;

	found = false;
	if ( statusCode == 200 || statusCode == 301) // succes code not just 200 - maybe make a success function
		return ;
	for ( size_t i = 0; i < serverConfig.error_pages.size(); i++ )
	{
		if ( serverConfig.error_pages[i].first == statusCode )
		{
			resource = serverConfig.error_pages[i].second;
			found = true;
		}
	}
	if ( found )
	{
		fillBody();
		finalBody = std::string( body.begin(), body.end() );
		// getContentType
		header.push_back( std::make_pair( "Content-Type", getContentType() ) );
	}
	else
	{
		resource.clear(); // temporary solution to know that i have default error page maybe change it
		fillDefaultErrorPage();
		header.push_back( std::make_pair( "Content-Type", "text/html" ) );
	}
}

void Response::response()
{
	protocol = "HTTP/1.1";
	setStatus();
	errorResponse();
	setHeaders();
}

std::string Response::result()
{
	std::string result;

	result = protocol + " " + std::to_string( statusCode ) + " " + status + "\r\n";
	if ( !cgiExist )
	{
		for ( size_t i = 0; i < header.size(); i++ )
		{
			result += header[i].first + ": " + header[i].second + "\r\n";
		}
		result += "\r\n";
	}
	// check if the cgi script has content-length maybe
	result += finalBody;
	clear();
	return ( result );
}

void Response::clear()
{
	protocol.clear();
	statusCode = 0;
	status.clear();
	header.clear();
	body.clear();
	finalBody.clear();
	resource.clear();
	cgiExist = false;
}

void Response::setup( const Request & request, const ServerInfo & serverInfo )
{
	statusCode = request.get_statusCode();
	setServerConfig( request, serverInfo );
	try {
		if ( statusCode != 200 )
			throw ( statusCode );
		std::cout << "enter response\n";
		urlHandle( request.get_path() );
		redirectionHandle();
		methodHandle( request );
		resourceHandle();
		directoryHandle( request );
		cgiHandle( request );
		fileHandle( request );
		throw (200);
	}
	catch ( int i )
	{
		statusCode = i;
		std::cout << "statusCode: " << statusCode << std::endl;
		response();
		throw ( 1 );
	}
}

// check location url
// check redirection
// check protocol
// check method and start going to what method you have
// check resource type and if it exist
