#include "webserv.hpp"
// he will need the body as an input file
// script path
// check extension of the script
// environment

Cgi::Cgi() {
	argv = NULL;
	env = NULL;
	path = NULL;
	pid = -1;
	timeout_duration = 25;
	timer_started = false;
	reading = false;
	done_reading = false;
	writing = false;
	done_writing = false;
	input_fd = -1;
	output_fd = -1;
	cgi_status = 200;
}

Cgi::Cgi( const Cgi & cgi ) {
	*this = cgi;
}

Cgi & Cgi::operator=( const Cgi & cgi ) {
	// be careful of pointers
	if ( this != &cgi ) {
		this->cgi_status = cgi.cgi_status;
		this->pid = cgi.pid;
		this->input_fd = cgi.input_fd;
		this->output_fd = cgi.output_fd;
		this->status = cgi.status;
		this->wait_result = cgi.wait_result;
		this->pid = cgi.pid;
		if ( cgi.path ) {
			this->path = strdup( cgi.path );
		}
		this->argv = doublePointerCopy( cgi.argv );
		this->env = doublePointerCopy( cgi.env );
		this->scriptName = cgi.scriptName;
		this->cgiDir = cgi.cgiDir;
		strcpy( this->rbuff ,cgi.rbuff );
		this->bytesRead = cgi.bytesRead;
		this->bytesWrites = cgi.bytesWrites;
		this->body = cgi.body;
		this->envList = cgi.envList;
		this->timeout_duration = cgi.timeout_duration;
		this->start_time = cgi.start_time;
		this->current_time = cgi.current_time;
		this->elapsed_time = cgi.elapsed_time;
		this->timer_started = cgi.timer_started;
		this->reading = cgi.reading;
		this->done_reading = cgi.done_reading;
		this->writing = cgi.writing;
		this->done_writing = cgi.done_writing;
		this->tmp = cgi.tmp;
	}
	return ( *this );
}

Cgi::~Cgi() {
	cgi_status = 200;
	pid = -1;
	body.clear();
	envList.clear();

	if ( env ) {
		for ( int i = 0; env[ i ]; i++ )
		{
			delete[] env[ i ]; 
		}
	}
	if ( argv ) {
		for ( int i = 0; argv[ i ]; i++ )
		{
			delete[] argv[ i ]; 
		}
	}
	delete[] env;
	delete[] argv;
	delete path;
	env = NULL;
	argv = NULL;
	path = NULL;
	reading = false;
	done_reading = false;
	writing = false;
	done_writing = false;
}

int Cgi::doublePointerLength( char **arr ) {
	int i = 0;

	while ( arr && arr[ i ] ) {
		i++;
	}
	return ( i );
}

char **Cgi::doublePointerCopy( char **src ) {
	int len;
	char **dst;
	int i = 0;

	len = doublePointerLength( src );
	dst = new char*[ len + 1 ];
	while ( i < len ) {
		dst[ i ] = strdup( src[ i ] );
		i++;
	}
	dst[ i ] = NULL;
	return ( dst );
}

int Cgi::get_status() const {
	return ( cgi_status );
}

int Cgi::get_output_fd() const {
	return ( output_fd );
}

int Cgi::get_input_fd() const {
	return ( input_fd );
}

std::string Cgi::get_body() const {
	return ( body );
}

std::string Cgi::strToUpper( const std::string & str )
{
	std::string result;

	for ( size_t i = 0; i < str.length(); i++ ) {
		result += std::toupper( str.at( i ) );
	}
	return ( result );
}

void Cgi::fill_output( int &kq ) 
{
	std::cout << "filling output" << std::endl;
	bytesRead = read( output_fd, rbuff, sizeof( rbuff ) - 1 );
	std::cout << "bytesRead: " << bytesRead << std::endl;
	if ( bytesRead < 0 ) {
		// FD_CLR( output_fd, &tmp_readfds );
		EV_SET(&changeEvent, output_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		if (kevent(kq, &changeEvent, 1, NULL, 0, NULL) == -1) {
			perror("kevent: fill_output");
			throw( 502 );
		}
		cgi_status = 502;
		perror("read");
		return ;
	}
	else if ( bytesRead == 0 ) {
		done_reading = true;
		reading = false;
		EV_SET(&changeEvent, output_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		if (kevent(kq, &changeEvent, 1, NULL, 0, NULL) == -1) {
			perror("kevent: fill_output");
			throw( 502 );
		}
		std::cout << "FInished reading" << std::endl;
		close ( output_fd );
		return ;
	}
	std::cout << "End of fill_output" << std::endl;
	rbuff[ bytesRead ] = '\0';
	body += rbuff;
}

void Cgi::fill_input( int &kq ) 
{
	std::cout << "filling input" << std::endl;
	bytesWrites = write( input_fd, tmp.c_str(), tmp.size() );
	if ( bytesWrites < 0 ) {
		EV_SET(&changeEvent, input_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		if (kevent(kq, &changeEvent, 1, NULL, 0, NULL) == -1) {
			perror("kevent: fill_InPut:");
			throw( 502 );
		}
		cgi_status = 502;
		perror("write");
		return ;
	}
	done_writing = true;
	EV_SET(&changeEvent, input_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &changeEvent, 1, NULL, 0, NULL) == -1) {
		perror("kevent: fill_Input:");
		throw( 502 );
	}
}

bool Cgi::running() {
	if ( pid != -1 ) {
		return ( true );
	}
	return ( false );
}

void Cgi::check_timeout() {
	current_time = time( NULL );
	elapsed_time = current_time - start_time;
  if (elapsed_time >= timeout_duration) {
		std::cout << "timeout_duration "<< timeout_duration << std::endl;
		std::cout << "Timeout occurred after " << elapsed_time << " seconds." << std::endl;
		kill(pid, SIGKILL);
		throw ( 504 );
	}
}

void Cgi::check_writing() {
	if ( done_writing ) {
		if (lseek(input_fd, SEEK_SET, 0) == -1) {
			perror("lseek");
			throw ( 500 );
		}
		return ;
	}
	throw ( 2 );
}

void Cgi::check_reading() {
	if ( reading ) {
		throw ( 2 );
	}
	if ( done_reading )	{
		close( output_fd );
		throw ( 200 );
	}
	return ;
}

void Cgi::is_done( int &kq ) {
	check_timeout();
	check_reading();
	waiting( kq );
}

void Cgi::check( int &kq ) {
	if ( cgi_status != 200 ) {
		throw ( 502 );
	}
	if ( running() ) {
		is_done( kq );
	}
	else {
		check_writing();
		launch( kq );
	}
}

void Cgi::setupEnv( const Request & request, const Response & response )
{
	envList.push_back("REQUEST_METHOD=" + strToUpper( request.get_method() )); 
	if ( request.headerExist( "content-type" ) )
		envList.push_back( "CONTENT_TYPE=" + request.getHeader( "content-type" ) );
	envList.push_back("PATH_INFO=" + request.get_path() );
	if ( !response.get_location().get_upload().empty() )
		envList.push_back( "UPLOAD_PATH=" + response.get_location().get_upload() );
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

	setCgiDir( response.get_resource() );
	setScriptName( response.get_resource() );
	if ( chdir( cgiDir.c_str() ) == -1 ) {
		perror("chdir: ");
		throw ( 502 );
	}
	path = new char[ strPath.length() + 1 ];
	std::strcpy( path, strPath.c_str() );
	argv = new char*[ 3 ]; 
	argv[0] = new char[ program.length() + 1 ];
	std::strcpy( argv[0], program.c_str() );
	argv[1] = new char[ scriptName.length() + 1 ];
	std::strcpy( argv[1], scriptName.c_str() );
	argv[2] = NULL;
}

void Cgi::setupInputFile( const Request & request, int &kq )
{
	std::string str;
    FILE* tempFile = tmpfile();

    if (tempFile == NULL) {
        perror("creating temp file");
		throw ( 502 );
    }
    input_fd = fileno(tempFile);
	if ( input_fd == -1 ) {
		perror("tmpfile");
		throw ( 502 );
	}
	set_nonblocking( input_fd );
	tmp = std::string( request.get_body().data(), request.get_body().size() );
	// FD_SET( input_fd, &tmp_writefds );
	// nfds = std::max(input_fd, nfds);
	EV_SET(&changeEvent, input_fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	if(kevent(kq, &changeEvent, 1, NULL, 0, NULL) == -1) {
		perror("kevent: SetupInputFile");
		throw ( 502 );
	}
	writing = true;
}

void Cgi::setupOutputFile()
{
	FILE* tempFile = tmpfile();

	if ( tempFile == NULL ) {
		throw ( 502 );
	}
	output_fd = fileno( tempFile );
	if ( output_fd == -1 ) {
		perror("out tmpfile");
		throw ( 502 );
	}
	set_nonblocking( output_fd );
}

void Cgi::setup( const Request & request, const Response & response, int &kq )
{
	std::cout << "setup\n";
	setupArgv( response );
	setupEnv( request, response );
	setupOutputFile();
	setupInputFile( request, kq );
	throw ( 2 );
}

// maybe put the input file in a pipe
void Cgi::waiting( int &kq ) {
	//std::cout << "waiting...\n";
	wait_result = waitpid( pid, &status, WNOHANG );
	if ( wait_result == 0 ) {
		throw ( 2 );
	}
	else if ( wait_result == pid ) {
		if (WIFEXITED(status)) { // Check if the child exited normally
			if ( WEXITSTATUS(status) != 0 ) {
				std::cout << "Hello 502\n";
				throw ( 502 );
			}
			//std::cout << "dony\n";
			//exit(0);
		}
		else if (WIFSIGNALED(status)) {
			throw ( 502 );
		}
		std::cout << "done\n";
		close( input_fd );
		if (lseek(output_fd, SEEK_SET, 0) == -1) {
			perror("lseek");
			throw ( 502 );
		}
		// FD_SET( output_fd, &tmp_readfds );
		// nfds = std::max(output_fd, nfds);
		EV_SET(&changeEvent, output_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(kq, &changeEvent, 1, NULL, 0, NULL) == -1) {
			perror("kevent in CGI::waiting");
			throw( 502 );
		}
		reading = true;
		throw ( 2 );
	}
	else {
		perror("wait: ");
		throw ( 502 );
	}
}

void Cgi::launch( int &kq )
{
	std::cout << "forking\n";
	pid = fork();
	if ( pid == 0 )
	{
		if ( dup2( input_fd, 0 ) == -1 ) {
			perror( "indupi" );
			throw ( "failed" );
			exit(1);
		}
		if ( dup2( output_fd, 1 ) == -1 ) {
			perror( "outdup" );
			throw ( "failed" );
			exit(1);
		}
		close( output_fd );
		close( input_fd );
		if ( execve( path, argv, env ) == -1 )
		{
			perror( "execve" );
			throw ( "failed" );
			exit(1);
		}
	}
	start_time = time( NULL );
	waiting( kq );
}

void Cgi::clear()
{
	cgi_status = 200;
	pid = -1;
	body.clear();
	envList.clear();

	if ( env ) {
		for ( int i = 0; env[ i ]; i++ )
		{
			delete[] env[ i ]; 
		}
	}
	if ( argv ) {
		for ( int i = 0; argv[ i ]; i++ )
		{
			delete[] argv[ i ]; 
		}
	}
	delete[] env;
	delete[] argv;
	delete path;
	env = NULL;
	argv = NULL;
	path = NULL;
	reading = false;
	done_reading = false;
	writing = false;
	done_writing = false;
}

