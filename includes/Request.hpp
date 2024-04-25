#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <vector>

class Request
{
	public:
		int statusCode;
		std::string method;
		std::string path;
		std::string protocol;
		std::vector<std::pair<std::string, std::string> > header;
		std::vector<char> body;
		std::vector<std::string> lines;	
		int index;
		std::string rem;
		bool inBody;
		std::vector<char> buffer;
		size_t contentLength;
		Request();
		Request( const Request & request );
		Request & operator=( const Request & request );
		~Request();
		// header-begin
		bool isUpperCaseLetters( const std::string & str );
		void handleMethod( const std::string & str );
		void handlePath( const std::string & str );
		void handleProtocol( const std::string & str );
		void handleRequestLine();
		std::string getKey( const std::string & str );
		std::string getValue( const std::string & str );
		void handleHeaders();
		void checkHeaderInfo();
		void isEnd();
		void parse();
		void headerFill();
		// header-end
		// body-begin
		void bodyFill();
		void setup( std::vector<char> & newBuffer );
		std::string getHeader( const std::string & key ) const;
		bool headerExist( const std::string & key ) const;
		void handleContentLength();
		// body-end
		// chunk-start
		int hexadecimalToDecimal(const std::string& hexadecimalString);
		void checkChunked();
		void parseChunked();
		void handleChunked();
		int chunkIndex;
		std::pair<std::string, std::string> chunkPair;
		std::string chunkBuffer;
		size_t chunkLength;
		// chunk-end
		// tmp
		void clear();
		void print();
};

// path_info
// query_string
// 

#endif
