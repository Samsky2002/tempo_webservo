#ifndef CGI_HPP
# define CGI_HPP

class Response;

class Cgi
{
	private:
		int cgi_status;
		int input_fd;
		int output_fd;
		int status;
		pid_t wait_result;
		pid_t pid;
		char *path;
		char **argv;
		char **env;
		std::string scriptName;
		std::string cgiDir;
		char rbuff[ 1024 ];
		int bytesRead;
		int bytesWrites;
		std::string body;
		std::vector<std::string> envList;
		/* timeout */
		time_t timeout_duration;
		time_t start_time;
		time_t current_time;
		time_t elapsed_time;
		bool timer_started;
		bool reading;
		bool done_reading;
		bool writing;
		bool done_writing;
		std::string request_body;
	public:
		Cgi();
		Cgi( const Cgi & cgi );
		Cgi & operator=( const Cgi & cgi );
		~Cgi();
		int get_status() const;
		int get_output_fd() const;
		int get_input_fd() const;
		int doublePointerLength( char **arr );
		char **doublePointerCopy( char **src );
		void fill_input( fd_set & tmp_readfds );
		void fill_output( fd_set & tmp_writefds );
		void check_writing();
		void check_reading();
		bool running();
		void check_timeout();
		void is_done( fd_set & tmp_readfds, int & nfds );
		void check_read();
		void check( fd_set & tmp_readfds, int & nfds );
		std::string get_body() const;
		void setup( const Request & request, const Response & response, fd_set & tmp_writefds, int & nfds );
		void launch( fd_set & tmp_readfds, int & nfds );
		std::string strToUpper( const std::string & str );
		void setCgiDir( const std::string & str );
		void setScriptName( const std::string & str );
		void setupArgv( const Response & response );
		void setupEnv( const Request & request, const Response & response );
		void setupInputFile( const Request & request, fd_set & tmp_writefds, int & nfds );
		void setupOutputFile();
		void waiting( fd_set & tmp_readfds, int & nfds );
		void clear();
};

#endif
