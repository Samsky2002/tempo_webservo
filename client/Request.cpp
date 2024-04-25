#include "webserv.hpp"

Request::Request()
{
	statusCode = 0;
	index = 0;
	chunkIndex = 0;
	inBody = false;
}

Request::Request( const Request & request )
{
	*this = request;
}

Request & Request::operator=( const Request & request )
{
	if ( this != &request )
	{
		//std::cout << "copy assignment request\n";
		this->statusCode = request.statusCode;
		this->method = request.method;
		this->path = request.path;
		this->protocol = request.protocol;
		this->header = request.header;
		this->body = request.body;
		this->lines = request.lines;
		this->index = request.index;
		this->rem = request.rem;
		this->inBody = request.inBody;
		this->buffer = request.buffer;
	}
	return ( *this );
}

Request::~Request()
{

}

// i need to check if it reached the end
// when the header is finished i need to parse and see if the necessary is there
bool Request::isUpperCaseLetters( const std::string & str )
{
	for ( size_t i = 0; i < str.length(); i++ )
	{
		if ( !std::isupper( str.at( i ) ) || !std::isalpha( str.at( i ) ) )
			return ( false );
	}
	return ( true );
}

std::vector<std::string> split( const std::string str )
{
	std::stringstream ss;
	std::string tmp;
	std::vector<std::string> result;

	ss << str;
	while ( ss >> tmp )
	{
		result.push_back( tmp );
	}
	return ( result );
}

void Request::handleMethod( const std::string & str )
{
	if ( !isUpperCaseLetters( str ) )
	{
		std::cerr << "not upper and letter\n";
		throw ( 400 );
	}
	method = str;
}

void Request::handlePath( const std::string & str )
{
	if ( str.at( 0 ) != '/' )
	{
		std::cerr << "path not starting with /\n";
		throw ( 400 );
	}
	path = str;
}

void Request::handleProtocol( const std::string & str )
{
	if ( str != "HTTP/1.1" )
		throw ( 505 );
	protocol = str;
}

// to be improved
void Request::handleRequestLine()
{
	std::vector<std::string> result;

	if ( index != 0 || lines[ index ] == "\r\n" )
		return ;
	result = split( lines[ index ] );
	if ( result.size() < 3 )
	{
		std::cerr << "less than requestline\n";
		throw ( 400 );
	}
	handleMethod( result[ 0 ] );
	handlePath( result [ 1 ] );
	handleProtocol( result[ 2 ] );
	if ( result.size() != 3 )
	{
		std::cerr << "more than requestline\n";
		throw ( 400 );
	}
}

std::string Request::getKey( const std::string & str )
{
	std::string key;
	std::size_t pos;

	pos = str.find( ":" );
	if ( pos != std::string::npos )
	{
		key = str.substr(0, pos);
	}
	else
	{
		key = str;
	}
	std::transform( key.begin(), key.end(), key.begin(), ::tolower );
	return ( key );
}

std::string Request::getValue( const std::string & str )
{
	std::string value;
	std::size_t pos;

	pos = str.find( ":" );
	if ( pos != std::string::npos )
		value = str.substr(pos + 1);
	//std::transform( value.begin(), value.end(), value.begin(), ::tolower );
	return ( value );
}

void Request::handleHeaders()
{
	if ( index == 0 || lines[ index ] == "\r\n" )
		return ;
	header.push_back( std::make_pair( getKey( lines[ index ] ), getValue( lines[ index ] ) ) );
}

std::string Request::getHeader( const std::string & key ) const
{
	for ( size_t i = 0; i < header.size(); i++ )
	{
		if ( header[ i ].first == key )
		{
			return ( header[ i ].second.c_str() );
		}
	}
	return ( std::string() );
}

bool Request::headerExist( const std::string & key ) const
{
	for ( size_t i = 0; i < header.size(); i++ )
	{
		if ( header[ i ].first == key )
		{
			return ( true );
		}
	}
	return ( false );
}

void Request::isEnd()
{
	if ( lines[ index ] == "\r\n" )
	{
		if ( index != 0 && lines[ index - 1] != "\r\n" )
		{
			checkHeaderInfo();
			if ( method != "POST" )	
				throw ( 200 );
			buffer.erase( buffer.begin() );
			contentLength = std::atoi( getHeader( "content-length" ).c_str() );
			inBody = true;
		}
	}
}

void Request::parse()
{
	handleRequestLine();
	handleHeaders();
	isEnd();
}

// i need to store all the headers in lower_case
// maybe add a host variable because it's gonna make it easy for me in response
// there is a problem with str
void Request::headerFill()
{
	std::string line;
	std::size_t pos;

	if ( inBody )
		return ;
	if ( !rem.empty() )
	{
		line = rem;
		rem.clear();
	}
	while ( !buffer.empty() )
	{
		line += buffer[ 0 ];
		pos = line.find( "\r\n" );
		if ( pos != std::string::npos )
		{
			lines.push_back( line );
			parse();
			index++;
			line.clear();
			if ( inBody )
				return ;
		}
		buffer.erase( buffer.begin() );
	}
	if ( !line.empty() )
	{
		rem = line;
	}
}

void printVecy( std::vector<char> & vec )
{
	for ( size_t i = 0; i < vec.size(); i++ )
	{
		std::cout << vec[i];
	}
}

void Request::handleContentLength()
{
	if ( !headerExist( "content-length" ) )
		return ;
	body.insert( body.end(), buffer.begin(), buffer.end() );
	if ( body.size() > contentLength )
	{
		std::cerr << "body content comparaison\n";
		throw ( 400 );
	}
	else if ( body.size() == contentLength )
		throw ( 200 );
}

int Request::hexadecimalToDecimal(const std::string& hexadecimalString) 
{
	std::stringstream ss;
    ss << std::hex << hexadecimalString;

    int decimalValue;
    ss >> decimalValue;

    if (ss.fail() || !ss.eof()) {
		std::cerr << "invalid chunk length\n";
        throw ( 400 );
    }
    return decimalValue;
}

void Request::checkChunked()
{
	if ( chunkLength != chunkPair.second.length() )
		throw ( 400 );
	if ( chunkLength == 0 )
		throw ( 200 );
	body.insert( body.end(), chunkPair.second.begin() ,chunkPair.second.end() ) ;
}

void Request::parseChunked()
{
	std::cout << "handling chunked\n";
	for ( size_t i = 0; i < buffer.size(); i++ )
	{
		chunkBuffer += buffer.at( i );
		if ( chunkBuffer.find( "\r\n" ) != std::string::npos )
		{
			chunkBuffer.erase( chunkBuffer.size() - 2 );
			if ( chunkIndex == 0 )
			{
				chunkPair.first = chunkBuffer;
				chunkLength = hexadecimalToDecimal( chunkPair.first );
				chunkIndex++;
			}
			else
			{
				chunkPair.second = chunkBuffer;
				chunkIndex++;
			}
			if ( chunkIndex == 2 )
			{
				checkChunked();
				chunkPair.first.clear();
				chunkPair.second.clear();
				chunkIndex = 0;
			}
			chunkBuffer.clear();
		}
	}
}

// if chunklength = 0 , end of chunked
// if the chunk is more than the chunk length throw bad request
// chunk is in hexa and if the hexa is wrong it's bad request

void Request::handleChunked()
{
	if ( !headerExist( "transfer-encoding" ) )
		return ;
	parseChunked();
	//printVecy( buffer );
}

void Request::bodyFill()
{
	if ( !inBody )
		return ;
	handleContentLength();
	handleChunked();
}
// check if content length or chunked

void Request::setup( std::vector<char> & newBuffer )
{
	buffer = newBuffer;
	try {
		//printVecy( buffer );
		headerFill();
		bodyFill();
	}
	catch ( int & e )
	{
		// set env variables
		print();
		statusCode = e;
		std::cout << e << std::endl;
		throw( e );
	};
}

void Request::print()
{
	std::cout << "method: " << method << std::endl;
	std::cout << "path: " << path << std::endl;
	std::cout << "protocol: " << protocol << std::endl;
	for ( size_t i = 0; i < header.size(); i++ )	
	{
		std::cout << "header: " << header[i].first << ":" << header[i].second;
	}
	//printVecy( body );
}

void Request::clear()
{
	std::cout << "clear\n";
	statusCode = 0;
	method.clear();	
	path.clear();
	protocol.clear();
	header.clear();
	body.clear();
	lines.clear();
	index = 0;
	rem.clear();
	inBody = false;
	buffer.clear();
}

// content length and the chunked
void Request::checkHeaderInfo()
{
	if ( !headerExist( "host" ) )
	{
		throw ( 400 );
	}
	else if ( method == "POST" )
	{
		if ( headerExist( "content-length" ) && headerExist( "transfer-encoding" ))
		{
			throw ( 400 );
		}
		else if ( !headerExist( "content-length" ) && !headerExist( "transfer-encoding" ) )
			throw ( 400 );
	}
}

// parse the request
// i need to start thinking about the response and how to handle it
// maybe do an enum for the error code that will go with a status code
// you should check the special characters allowed in http request
// the key headers can be anything , it dones't need : or value or something
// key always attached to : , the value doesn't matter
// key is necessary to have in case of ":value"
// key can only accept printable characters no space no tab no weird stuff
// value accept everything
