#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include <vector>
# include <sstream>
# include "Request.hpp"
# include "Cgi.hpp"

enum ResourceType
{
	Directory,
	File
};

class Response
{
	public:
		std::string protocol;
		int statusCode;
		std::string status;
		std::vector<std::pair<std::string, std::string> > header;
		std::vector<int> body;
		std::string finalBody;
		ServerConfig serverConfig;
		Location location;
		Cgi cgi;
		std::string resource;
		bool cgiExist;
		ResourceType resourceType;
		std::string responseBuffer;
		Response();
		Response( const Response & response );
		Response & operator=( const Response & response );
		~Response();
		void setup( const Request & request, const ServerInfo & serverInfo );
		void setServerConfig( const Request & request, const ServerInfo & serverInfo );
		bool is_url_found_in_path( const std::string & requestPath, const std::string & locationPath  );
		void urlHandle( std::string requestPath );
		void redirectionHandle();
		void methodHandle( const Request & request );
		void resourceHandle();
		bool hasIndex();
		bool hasIndexHtml();
		void autoIndex( const Request & request );
		void directoryHandle( const Request & request );
		bool isCgi();
		void cgiHandle( const Request & request );
		void fileHandle( const Request & request );
		void fillBody();
		bool hasTrailingSlach();
		void setStatus();
		std::string getExtension();
		std::string getContentType();
		void setHeaders();
		void fillDefaultErrorPage();
		void errorResponse();
		void response();
		std::string result();
		void clear();
};


// give the handler what he need to check

#endif
