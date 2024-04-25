#include "webserv.hpp"
// he will need the body as an input file
// script path
// check extension of the script
// environment

std::string strToUpper( const std::string & str )
{
	std::string result;

	for ( size_t i = 0; i < str.length(); i++ ) {
		result += std::toupper( str.at( i ) );
	}
	return ( result );
}

void Cgi::setupEnv( const Request & request, const Response & response )
{
	// i need to use new
	std::cout << request.body.size() << std::endl;
	envList.push_back("REQUEST_METHOD=" + strToUpper( request.method )); 
	if ( request.headerExist( "content-type" ) )
		envList.push_back( "CONTENT_TYPE=" + request.getHeader( "content-type" ) );
	envList.push_back("PATH_INFO=" + request.path );
	// upload path
	if ( !response.location.upload.empty() )
		envList.push_back( "UPLOAD_PATH=" + response.location.upload );
	env = new char*[ envList.size() + 1 ];
	for ( size_t i = 0; i < envList.size(); i++ )
	{
		env[ i ] = new char[ envList[i].size() + 1 ];
		std::strcpy( env[ i ], envList[ i ].c_str());
	}
	env[ envList.size() ] = NULL;
}
// query_string
// content_length
// script_name
// 

// i need relative path and run the script inside the directory
void Cgi::setCgiDir( const std::string & str )
{
	size_t pos = str.find_last_of( '/' );	

	if ( pos != std::string::npos )
		cgiDir = str.substr(0, pos);
}

void Cgi::setScriptName( const std::string & str )
{
	size_t pos = str.find_last_of( '/' );	

	if ( pos != std::string::npos )
		scriptName = str.substr(pos + 1);
}

void Cgi::setupArgv( const Response & response )
{
	std::string strPath = "/usr/local/bin/python3";
	std::string program = "python3";

	setCgiDir( response.resource );
	setScriptName( response.resource );
	if ( chdir( cgiDir.c_str() ) == -1 )
		perror("chdir: ");
	path = new char[ strPath.length() + 1 ];
	std::strcpy( path, strPath.c_str() );
	argv = new char*[ 3 ]; 
	argv[0] = new char[ program.length() + 1 ];
	std::strcpy( argv[0], program.c_str() );
	argv[1] = new char[ scriptName.length() + 1 ];
	std::strcpy( argv[1], scriptName.c_str() );
	argv[2] = NULL;
}

void Cgi::setupInputFile( const Request & request )
{
	std::string str;
    FILE* tempFile = tmpfile();

    if (tempFile == NULL) {
        perror("creating temp file");
        return ;
    }
    // Get the file descriptor associated with the FILE*
    input_fd = fileno(tempFile);
	if ( input_fd == -1 )
		perror("tmpfile");
	std::string tmp( request.body.begin(), request.body.end() );
	write( input_fd, tmp.c_str(), tmp.size() );
	if (lseek(input_fd, SEEK_SET, 0) == -1) {
        perror("fcntl");
        exit(1);
    }
}
void Cgi::setupOutputFile()
{
	FILE* tempFile = tmpfile();

	if ( tempFile == NULL ) {
		perror("creating tmp file");
		return ;
	}
	output_fd = fileno( tempFile );
	if ( output_fd == -1 )
		perror("out tmpfile");
}

void Cgi::setup( const Request & request, const Response & response )
{
	setupArgv( response );
	setupEnv( request, response );
	setupInputFile( request );
	setupOutputFile();
}

// maybe put the input file in a pipe
void Cgi::launch()
{
	pid = fork();
	std::cout << "forking\n";
	if ( pid == 0 )
	{
		if ( dup2( input_fd, 0 ) == -1 )
			perror( "indup" );
		if ( dup2( output_fd, 1 ) == -1 )
			perror( "outdup" );
		close( output_fd );
		close( input_fd );
		if ( execve( path, argv, env ) == -1 )
		{
			perror( "execve" );
			exit(1);
		}
	}
	close( input_fd );
	std::cout << "waiting...\n";
	if ( wait( NULL ) == -1 )
		perror("wait: ");
	if (lseek(output_fd, SEEK_SET, 0) == -1) {
        perror("lseek");
        exit(1);
    }
	std::cout << "reading\n";
	while ( ( bytesRead = read( output_fd, rbuff, sizeof( rbuff ) - 1 ) ) > 0 )
	{
		rbuff[ bytesRead ] = '\0';
		body += rbuff;
	}
	std::cout << body << std::endl;
	close ( output_fd );
}

void Cgi::clear()
{
	body.clear();
	envList.clear();
	for ( int i = 0; env[ i ]; i++ )
	{
		delete[] env[ i ]; 
	}
	for ( int i = 0; argv[ i ]; i++ )
	{
		delete[] argv[ i ]; 
	}
	delete[] env;
	delete[] argv; 
}

