/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndahib <ndahib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 08:07:29 by ndahib            #+#    #+#             */
/*   Updated: 2024/04/29 10:01:01 by ndahib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

/* ************************************************************************** */
/*               ORTHODOX_CANONICAL_FORM                                      */
/* ************************************************************************** */

Request::Request() : statusCode(0)
{
	// Default Constructor;
}

Request::Request(const Request &obj)
{
	(void)obj;
}

Request &Request::operator=(const Request &obj)
{
	if (this != &obj)
	{
		statusCode = obj.statusCode;
		buffer = obj.buffer;		
		path = obj.path;
		method = obj.method;
		protocol = obj.protocol;
		requestLine = obj.requestLine;
		fragement = obj.fragement;
		query = obj.query;
		path = obj.path;
		body = obj.body;
	}
	return (*this);
}

Request::~Request(){}

/* ************************************************************************** */
/*               Outils                                   		  			  */
/* ************************************************************************** */

void	toLower(std::string &key)
{
	for (size_t i = 0; i < key.size(); i++)
	{
		if (std::isdigit(key[i]))
			throw (400);
		if ((key[i] != '-' && !std::isalpha(key[i])))
			throw(400);
		key[i] = std::tolower(key[i]);
	}
}

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


/* ************************************************************************** */
/*               Methods                                     				  */
/* ************************************************************************** */

void Request::clear()
{
	std::cout << "clear\n";
	statusCode = 0;
	method.clear();	
	path.clear();
	protocol.clear();
	header.clear();
	body.clear();
	requestLine.clear();
	buffer.clear();
	fragement.clear();
	Finalpath.clear();
	query.clear();
	body.clear();
	Finalbody.clear();
	buffer.clear();
	TmpHeaders.clear();
	chunkBuffer.clear();
}

void	Request::split_request()
{
	size_t pos = 0;
	std::string line;
	
	// Parse request line
	while (pos < buffer.size() && buffer[pos] != '\r' && buffer[pos + 1] != '\n') 
	{
		requestLine += buffer[pos];
		++pos;
	}
	pos += 2;

	// Parse headers
	while (pos < buffer.size()) 
	{
		while (buffer[pos] != '\r')
		{
			line += buffer[pos];
			pos++;
		}
		TmpHeaders.push_back(line);
		line.clear();
		if (std::string(buffer.begin() + pos, buffer.begin() + pos + 4) == "\r\n\r\n")
			break;
		pos += 2;
	}
	pos += 2;

	// Copy the body
	while (pos < buffer.size()) 
	{
		body.push_back(buffer[pos]);
		++pos;
	}
}

void	Request::print()
{
	std::cout << " ****************************Request Line***********************************" << std::endl;
	std::cout << "Request Line->:" << requestLine << std::endl;
	std::cout << "Method-->		:" << method << std::endl;
	std::cout << "Path---->		:" << path << std::endl;
	std::cout << "Prinintg the query if Exist---->>:" << std::endl;
	for(std::map<std::string, std::string>::iterator it = query.begin();
		it != query.end(); ++it)
	{
		std::cout << it->first << "=" << it->second << "\n";
	}  
	std::cout << "Fragement---->:" << fragement << std::endl;
	std::cout << "Protocol->	:" << protocol << std::endl;
	std::cout << " ******************************Headre*************************************" << std::endl;
	for (size_t i = 0; i < header.size(); i++)
	{
		std::cout << "===key :[" << header[i].first << "] = " << header[i].second << std::endl; 
	}
	std::cout << " ********************************Body**************************************" << std::endl;
	for (size_t i = 0; i < body.size(); i++)
	{
		std::cout << body[i];
	}
}


bool Request::headerExist( const std::string & key ) const
{
	for (size_t i = 0; i < header.size(); i++)
	{
		if (header[i].first == key)
			return (true);	
	}
	return (false);
}

std::string const Request::getHeader(const std::string & key ) const
{
	for (size_t i = 0; i < header.size(); i++)
	{
		if (header[i].first == key)
			return (header[i].second);	
	}
	return ("");
}

bool	Request::HostAlreadyExist()
{
	for (size_t i = 0; i < header.size(); i++)
	{
		if (header[i].first == "host")
			return (true);
	}
	return (false);
}
void Request::checkChunked()
{
	if ( chunkLength != chunkPair.second.length() )
		throw ( 400 );
	if ( chunkLength == 0 )
	{
		Finalbody.insert( Finalbody.end(), chunkPair.second.begin() ,chunkPair.second.end() ) ;
		throw ( 200 );
	}
}

void print_buffer(std::vector<char> body)
{
	for ( size_t i = 0; i < body.size(); i++ )
		std::cout << body[i];
}

void	Request::handleChunked()
{
	int chunkIndex = 0;
	for ( size_t i = 0; i < body.size(); i++ )
	{
		chunkBuffer += body[i];
		if ( chunkBuffer.find( "\r\n" ) != std::string::npos )
		{
			chunkBuffer.pop_back();
			chunkBuffer.pop_back();
			if ( chunkIndex == 0 )
			{
				if (chunkBuffer.empty() == true)
					continue;
				chunkPair.first = chunkBuffer;
				chunkLength = ConvertHex( chunkPair.first );
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
	body.clear();
	body = Finalbody;
}

/* ************************************************************************** */
/*					Parsing Methods											  */
/* ************************************************************************** */

void	Request::parse_path()
{
	//decoding path
	size_t found;
	while  ((found = path.find('%')) != std::string::npos)
	{
		path.replace(found, 3, 1, HexToChar(path.substr(found + 1, 2)));
	}
	std::stringstream	ss(path);	

	// Splitting Path
	getline(ss, Finalpath, '?');
	std::string part;
	size_t	del;
	size_t	fragement_found;
	while (std::getline(ss, part, '?') && part.empty() == false) 
	{
		fragement_found = part.find( '#' );
		if ((del = part.find( '=' )) == std::string::npos)
		{
			throw(400);
		}
		query[part.substr(0, del)] = part.substr(del + 1, part.size() - fragement_found + 1);
		if (fragement_found != std::string::npos)
		{
			//fragement exit;
			fragement = part.substr(fragement_found + 1);
			break;
		}
	}
	if (path.size() > MAX_SIZE_LENGHT)
		throw(414);
	path = Finalpath;
}

void	Request::parse_request_line()
{
	std::vector< std::string> token = split(requestLine);

	if (token.empty() || token[1][0] !=  '/' )
	{
		throw (400); // Bad Request - Empty Request Line
	}
	if (token.size() < 3 )
		throw (404);
	if (token[0] == "GET")
		method = "GET";
	else if (token[0] == "POST")
		method = "POST";
	else if (token[0] == "DELETE")
		method = "DELETE";
	else throw(501); 	// NOn Standard Methods 501 NOT IMplemeted 
	
	// Passing to the Syntax;
	if (token[2] != "HTTP/1.1")
		throw(505);   // Unsupported version , 505 HTTP Version Not Supported).
	path = token[1];
	protocol = token[2];
	parse_path();
}

void	Request::parse_headers()
{
	std::string	key;
	std::string	value;
	size_t	found_del;
	
	std::map<std::string, std::string>	Headers;
	for (size_t i = 0; i < TmpHeaders.size(); i++)
	{
		if ((found_del = TmpHeaders[i].find(':' )) == std::string::npos)
		{
			throw(400);
		}
		key = TmpHeaders[i].substr(0, found_del);
		value = TmpHeaders[i].substr(found_del + 2, TmpHeaders[i].size() - 3);
		
		toLower(key);
		if (key == "host" && HostAlreadyExist() == true)
		{
			throw(400);
		}
		Headers[key] = value;
		header.push_back(std::make_pair(key, value));
	}
	if (method == "GET" && headerExist("host") == false)
		throw(404);
}

void	Request::parse_body()
{
	if (headerExist("centent-lenght") == true && headerExist("transfer-encoding") == true)
		throw(400);
	if (body.empty() == false)
	{
		if (headerExist( "content-lenght" ))
		{
			if (std::atoi(getHeader("content-lenght").c_str()) != (int)body.size())
				throw(400);
			else
				throw(200);
		}
		else if (headerExist( "transfer-encoding" ))
		{
			handleChunked();
		}
		else if (strncmp(body.data(), "\r\n", body.size()) != 0)
		{
			// Compare if the body just CLRF
			throw(411);
		}
		else
			throw(200);
	}
}

void	Request::parse_request()
{
	parse_request_line();
	parse_headers();
	parse_body();
}

/* ************************************************************************** */
/*				Setup											  			  */
/* ************************************************************************** */

void Request::setup( std::vector<char> & newBuffer )
{
	buffer = newBuffer;
	try
	{
		split_request();
		parse_request();
	}
	catch(const int &e)
	{
		print();
		std::cout << "The Request exit wiht " << e << std::endl;
		statusCode = e;
		throw(e);
	}
}
