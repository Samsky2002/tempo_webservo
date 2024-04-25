#include "webserv.hpp"

std::vector<std::string> split(const std::string& str, char c) {
    std::vector<std::string> result;
    std::string currentWord;
    bool inWord = false;

    for (size_t i = 0; i < str.length(); ++i) {
        char ch = str[i];

        if (ch == c) {
            if (inWord) {
                result.push_back(currentWord);
                currentWord.clear();
                inWord = false;
            }
        } else {
            currentWord += ch;
            inWord = true;
        }
    }
    if (inWord) {
        result.push_back(currentWord);
    }
    return result;
}
int	main( int argc, char **argv )
{
	std::string file;
	Http http;

	if ( argc > 2 )
		return ( 1 );
	file = argv[1] ? argv[1] : "config_file";
	http.execute( file );
	//http.setup(file);
	//http.launch();
	//maybe do an http exec
}
