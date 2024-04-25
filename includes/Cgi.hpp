#ifndef CGI_HPP
# define CGI_HPP

class Response;

class Cgi
{
	public:
		int input_fd;
		int output_fd;
		int pid;
		char *path;
		char **argv;
		char **env;
		std::string scriptName;
		std::string cgiDir;
		char rbuff[ 1024 ];
		int bytesRead;
		std::string body;
		std::vector<std::string> envList;
		void setup( const Request & request, const Response & response );
		void launch();
		//std::string strToUpper( const std::string & str );
		void setCgiDir( const std::string & str );
		void setScriptName( const std::string & str );
		void setupArgv( const Response & response );
		void setupEnv( const Request & request, const Response & response );
		void setupInputFile( const Request & request );
		void setupOutputFile();
		void clear();
};

#endif
