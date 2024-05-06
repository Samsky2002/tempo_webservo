#include "webserv.hpp"

Response::Response()
{
	cgiExist = false;
	statusCode = -1;
	cgiFailed = false;
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
		this->cgi = response.cgi;
		this->resource = response.resource;
		this->cgiExist = response.cgiExist;
		this->cgiFailed = response.cgiFailed;
		this->resourceType = response.resourceType;
		this->responseBuffer = response.responseBuffer;
	}
	return ( *this );
}

Response::~Response()
{

}

bool Response::headerExist( const std::string & key ) const {
	for ( size_t i = 0; i < header.size(); i++ ) {
		if ( header[ i ].first == key ) {
			return ( true );
		}
	}
	return ( false );
}

Location Response::get_location() const {
	return ( location );
}

Cgi & Response::get_cgi() {
	return ( cgi );
}

std::string Response::get_resource() const {
	return ( resource );
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

	if ( inputFile.is_open( ) ) {
		ss << inputFile.rdbuf();
		line = ss.str();
		body = std::vector<char>(line.begin(), line.end());
		inputFile.close();
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

void Response::urlHandle( std::string requestPath )
{
	bool found;

	for ( size_t i = 0; i < serverConfig.get_location().size(); i++ ) {
		found = is_url_found_in_path( requestPath, serverConfig.get_location_idx( i ).get_path() );
		if ( found ) {
			if ( !location.get_path().empty() ) {
				if ( location.get_path().length() < serverConfig.get_location_idx( i ).get_path().length() )
					location = serverConfig.get_location_idx( i );
			}
			else
				location = serverConfig.get_location_idx( i );
		}
	}
	if ( location.get_path().empty() )
	{
		std::cerr << "error: no matching location\n";
		throw ( 404 );
	}
	resource = requestPath.replace( 0, location.get_path().length(), location.get_root() );
}

void Response::redirectionHandle()
{
	if ( location.get_redirection().empty() )
		return ;
	header.push_back( std::make_pair( "Location", location.get_redirection() ) );
	throw ( 301 );
}

void Response::methodHandle( const Request & request )
{
	if ( request.get_method() != "GET" && request.get_method() != "POST" && request.get_method() != "DELETE" ) {
		throw ( 405 );
	}
	if ( location.get_methods().empty() )
		return ;
	for ( size_t i = 0; i < location.get_methods().size(); i++ ) {
		if ( request.get_method() == location.get_methods_idx( i ) )
			return ;
	}
	throw ( 405 );
}

void Response::setServerConfig( const Request & request, const ServerInfo & serverInfo )
{
	std::string host;

	host = request.getHeader( "host" );
	for ( size_t i = 0; i < serverInfo.get_serverConfig().size(); i++ )
	{
		for ( size_t j = 0; j <  serverInfo.get_serverConfig_idx( i ).get_server_names().size(); j++ ) {
			if ( serverInfo.get_serverConfig_idx( i ).get_server_names_idx( j ) == host )
			{
				serverConfig = serverInfo.get_serverConfig_idx( i );
				return ;
			}
		}
	}
	serverConfig = serverInfo.get_serverConfig_idx( 0 );
}

bool Response::file_exist( const char * str ) {
	if ( !access( str, F_OK ) ) {
		return ( true );
	}
	return ( false );
}

bool Response::hasIndexHtml()
{
	std::string tmp;

	tmp = resource + "index.html";
	if ( file_exist( tmp.c_str() ) ) {
		resource = tmp;
		return ( true );
	}
	return ( false );
}

bool Response::hasIndex()
{
	std::string tmp;

	if ( location.get_index().empty() )
	{
		if ( hasIndexHtml() )
			return ( true );
		return ( false );
	}
	for ( size_t i = 0; i < location.get_index().size(); i++ )
	{
		tmp = resource;
		tmp += location.get_index_idx( i );
		if ( !file_exist( tmp.c_str() ) )
		{
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
	if ( !hasTrailingSlach() )
	{
		header.push_back( std::make_pair( "Location", request.get_path() + "/" ) );
		throw ( 301 );
	}
	if ( hasIndex() )
		return ;
	if ( location.get_autoindex() )
		autoIndex( request );
	throw ( 403 );
}

bool Response::isCgi()
{
	if ( location.get_path() == "/cgi-bin" || location.get_path() == "/cgi-bin/" )
	{
		cgiExist = true;
		return ( true );
	}
	return ( false );
}

void Response::cgiHandle( const Request & request, fd_set & tmp_writefds, int & nfds )
{
	if ( !isCgi() )
		return ;
	if ( request.get_method() == "DELETE" ) {
		// this is temporary i should know the exact code
		throw ( 405 );
	}
	cgi.setup( request, *this, tmp_writefds, nfds );
	throw( 2 );
}

void Response::deleteFileHandle( const Request & request ) {
	if ( request.get_method() != "DELETE" ) {
		return ;
	}
	if ( remove( resource.c_str() ) == -1 ) {
		throw ( 500 );
	}
	// maybe do the body or not
	finalBody += "<head></head><body>Successful Delete</body>\n";
	throw ( 200 );
}

void Response::getFileHandle() {
	fillBody();
	finalBody = std::string( body.begin(), body.end() );
}
void Response::fileHandle( const Request & request )
{
	if ( request.get_method() == "POST" ) {
		throw ( 405 );
	}
	deleteFileHandle( request );
	getFileHandle();
}

void Response::setStatus()
{
	if ( statusCode == 200 )
		status = "OK";
	else if ( statusCode == 301 )
		status = "Moved Permanently";
	else if ( statusCode == 400 )
		status = "Bad Request";
	else if ( statusCode == 403 )
		status = "Forbidden";
	else if ( statusCode == 404 )
		status = "Not Found";
	else if ( statusCode == 405 )
		status = "Not Allowed";
	else if ( statusCode == 500 )
		status = "Internal Server Error";
	else if ( statusCode == 502 )
		status = "Bad Gateway";
	else if ( statusCode == 504 )
		status = "Gateway Timeout";
	else if ( statusCode == 505 )
		status = "HTTP Version Not Supported";
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
	else if ( extension == "txt" )
		return ( "text/plain" );
	else if ( extension == "css" )
		return ( "text/css" );
	else if ( extension == "js" )
		return ( "text/javascript" );
	else if ( extension == "mp4" )
		return ( "video/mp4" );
	else if ( extension == "mpeg" )
		return ( "video/mpeg" );
	else if ( extension == "mp3" )
		return ( "audio/mpeg" );
	else if ( extension == "php" )
		return ( "application/x-httpd-php" );
	else if ( extension == "ppt" )
		return ( "application/vnd.ms-powerpoint" );
	else if ( extension == "jpg" )
		return ( "image/jpeg" );
	else if ( extension == "jpeg" )
		return ( "image/jpeg" );
	else if ( extension == "png" )
		return ( "image/png" );
	else if ( extension == "pdf" )
		return ( "application/pdf" );
	else if ( extension == "avif" )
		return ( "image/avif" );
	else if ( extension == "gif" )
		return ( "image/gif" );
	else if ( extension == "ico" )
		return ( "image/x-icon" );
	return ( "text/plain" );
}

void Response::setHeaders()
{
	if ( cgiExist )
		return ;
	if ( !headerExist( "Content-Type" ) ) {
		header.push_back( std::make_pair( "Content-Type", getContentType() ) );
	}
	if ( !headerExist( "Content-Length" ) ) {
		header.push_back( std::make_pair( "Content-Length", std::to_string( finalBody.length() ) ) );
	}
}

void Response::fillDefaultErrorPage()
{
	finalBody = "<html>\n<head>\n\t<title>" + std::to_string( statusCode ) + " " + status + "</title>\n</head>\n";
	finalBody += "<body>\n<center>\n\t<h1>" + std::to_string( statusCode ) + " " + status + "</h1>\n</center>\n";
	finalBody += "<hr>\n<center>webserv/1.1</center>\n</body>\n</html>\n";
	header.push_back( std::make_pair( "Content-Type", "text/html" ) );
}

void Response::errorResponse()
{
	bool found;

	found = false;
	if ( statusCode == 200 || statusCode == 301) // succes code not just 200 - maybe make a success function
		return ;
	for ( size_t i = 0; i < serverConfig.get_error_pages().size(); i++ )
	{
		if ( serverConfig.get_error_pages_idx( i ).first == statusCode )
		{
			resource = serverConfig.get_error_pages_idx( i ).second;
			found = true;
		}
	}
	if ( found && file_exist( resource.c_str() ) )
	{
		// check if the error page is found
		fillBody();
		finalBody = std::string( body.begin(), body.end() );
		//header.push_back( std::make_pair( "Content-Type", getContentType() ) );
	}
	else
	{
		fillDefaultErrorPage();
	}
}

void Response::response()
{
	protocol = "HTTP/1.1";
	setStatus();
	errorResponse();
	setHeaders();
}

// to_string maybe c++11
std::string Response::result()
{
	std::string result;

	result = protocol + " " + std::to_string( statusCode ) + " " + status + "\r\n";
	if ( !cgiExist || cgiFailed) {
		for ( size_t i = 0; i < header.size(); i++ )
		{
			result += header[i].first + ": " + header[i].second + "\r\n";
		}
		result += "\r\n";
	}
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
	cgiFailed = false;
}

void Response::checkCgi( fd_set & tmp_readfds, int & nfds ) {
	try {
		cgi.check( tmp_readfds, nfds );
	}
	catch ( int i ) {
		if ( i == 200 ) {
			finalBody = cgi.get_body();
			cgi.clear();
		}
		else if ( i != 2 ) {
			cgiFailed = true;
		}
		throw ( i );
	}
}

void Response::setup( const Request & request, const ServerInfo & serverInfo, fd_set & tmp_readfds, fd_set & tmp_writefds, int & nfds )
{
	try {
		if ( cgiExist ) {
			checkCgi( tmp_readfds, nfds );
		}
		setServerConfig( request, serverInfo );
		if ( request.get_statusCode() != 200 )
			throw ( request.get_statusCode() );
		urlHandle( request.get_path() );
		redirectionHandle();
		methodHandle( request );
		resourceHandle();
		directoryHandle( request );
		cgiHandle( request, tmp_writefds, nfds );
		fileHandle( request );
		throw (200);
	}
	catch ( int i )
	{
		if ( i == 2 ) {
			throw ( 2 );
		}
		statusCode = i;
		response();
		throw ( 1 );
	}
}
// check location url
// check redirection
// check protocol
// check method and start going to what method you have
// check resource type and if it exist
