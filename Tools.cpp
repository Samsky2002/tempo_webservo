#include "webserv.hpp"


std::vector<std::string> split( const std::string str )
{
	std::stringstream			ss;
	std::string					tmp;
	std::vector<std::string>	result;

	ss << str;
	while ( ss >> tmp )
	{
		result.push_back( tmp );
	}
	return ( result );
}


int ConvertHex(const std::string& path) 
{
	int len = path.size();
	int base = 1;
	int dec_val = 0;
	for (int i = len - 1; i >= 0; i--) 
	{
		if (path[i] >= '0' && path[i] <= '9')
		{
			dec_val += (int(path[i]) - 48) * base;
			base = base * 16;
		} else if (path[i] >= 'A' && path[i] <= 'F')
		{
			dec_val += (int(path[i]) - 55) * base;
			base = base * 16;
		}else if (path[i] >= 'a' && path[i] <= 'f')
		{
			dec_val += (int(path[i]) - 87) * base;
			base = base * 16;
		}
	}
	return (dec_val);
}

char	HexToChar(const std::string& path)
{
	return ((char)( ConvertHex(path) ));
}


void	toLower(std::string &key)
{
	for (size_t i = 0; i < key.size(); i++)
	{
		if (std::isdigit(key[i]) || (key[i] != '-' && !std::isalpha(key[i])))
		{
			std::cout << "400 Bad Request : " << key <<std::endl;
			throw (400);
		}
		key[i] = std::tolower(key[i]);
	}
}
