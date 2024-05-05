/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndahib <ndahib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 08:07:29 by ndahib            #+#    #+#             */
/*   Updated: 2024/05/05 12:40:50 by ndahib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

/* ************************************************************************** */
/*               ORTHODOX_CANONICAL_FORM                                      */
/* ************************************************************************** */

Request::Request()
{
	statusCode = 0;
	index = 0;
	chunkIndex = 0;
	chunkLength = 0;
	inBody = false;
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
		fragement = obj.fragement;
		query = obj.query;
		path = obj.path;
		body = obj.body;
	}
	return (*this);
}

Request::~Request(){}

/* ************************************************************************** */
/*               Getters                                    				  */
/* ************************************************************************** */
std::string Request::get_method() const {
	return ( method );
}

std::string Request::get_path() const {
	return ( path );
}

int Request::get_statusCode() const {
	return ( statusCode );
}

std::vector< char > Request::get_body() const {
	return ( body );
}

std::string const Request::getHeader( const std::string & key ) const
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
	return (isExist(header, key));
}


/* ************************************************************************** */
/*					Parsing Methods											  */
/* ************************************************************************** */
void	Request::parse_path()
{
	size_t found;
	std::string Finalpath;

	while  ((found = path.find('%')) != std::string::npos)
	{
		path.replace(found, 3, 1, HexToChar(path.substr(found + 1, 2)));
	}
	std::stringstream	ss(path);	

	getline(ss, Finalpath, '?');
	std::string part;
	size_t	del;
	size_t	fragement_found;
	while (std::getline(ss, part, '?') && part.empty() == false) 
	{
		fragement_found = part.find( '#' );
		if ((del = part.find( '=' )) == std::string::npos)
		{
			throw(4001);
		}
		query[part.substr(0, del)] = part.substr(del + 1, part.size() - fragement_found + 1);
		if (fragement_found != std::string::npos)
		{
			fragement = part.substr(fragement_found + 1);
			break;
		}
	}
	if (path.size() > MAX_SIZE_LENGHT)
		throw(414);
	path = Finalpath;
}

void	Request::parseRequestLine()
{
	if ( inBody == true || index != 0 || lines[ index ] == "\r\n" )
		return ;
	std::vector<std::string> tokens = split(lines[index]);
	
	if (tokens.size() < 3 || tokens[0].empty() || tokens[1].empty() || tokens[2].empty()) 
	{
		throw (404);
	}
	method = tokens[0];
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw(501);
	if (tokens[2] != "HTTP/1.1")
		throw(505);
	path = tokens[1];
	protocol = tokens[2];
	parse_path();
}

void	Request::parseHeader()
{
	std::string	key;
	std::string	value;
	size_t		found_del;

	if ( inBody == true || index == 0 || lines[ index ] == "\r\n" ) return ;
	found_del = lines[index].find(':');
	key = lines[index].substr(0, found_del);
	value = lines[index].substr(found_del + 2, lines[index].size() - key.size() - 4);
	toLower(key);
	if (key == "host" && isExist(header, key) == true) 
	{
		throw(4002);
	}
	header.push_back(std::make_pair(key, value));
}


void	Request::parse()
{
	parseRequestLine();
	parseHeader();
	checkEndOfHeaders();
}

/* ************************************************************************** */
/*               Methods                                     				  */
/* ************************************************************************** */
void Request::checkEndOfHeaders()
{
	if (index != 0 && lines[index] == "\r\n" && lines[index - 1] != "\r\n")
	{
		::print(buffer);
		if (method != "POST")
		{
			(method == "GET" && !headerExist( "host" )) ? throw(4003) : throw(200);
		}
		else
		{
			if ((headerExist( "content-length" ) && headerExist( "transfer-encoding" ))||
				(!headerExist( "content-length" ) && !headerExist( "transfer-encoding" )) )
			{
				throw(4004);
			}
		}
		inBody = true;
	}
}

void	Request::headerFill()
{
	std::string	currentLine;
	std::size_t lineEndPos;

	if (inBody == true)
		return;
	if ( !rem.empty() )
	{
		currentLine = rem;
		rem.clear();
	}
	while (!buffer.empty())
	{
		currentLine += buffer[ 0 ];
		lineEndPos = currentLine.find("\r\n");
		buffer.erase(buffer.begin());
		if (lineEndPos != std::string::npos)
		{
			lines.push_back(currentLine);
			currentLine.clear();
			parse();
			if (inBody == true)
				return ;
			index++;
		}
	}
	if ( !currentLine.empty() )
	{
		rem = currentLine;
	}
}

void	Request::handleContentLength()
{
	body.insert( body.end(), buffer.begin(), buffer.end());
	std::cout << "content length: " << getHeader("content-length")  << "===>>" << body.size() << std::endl;
	if ( (int)body.size() != std::atoi(getHeader("content-length").c_str()) )
	{
		throw ( 4005 );
	}
	else if ( (int)body.size() == std::atoi(getHeader("content-length").c_str()))
		throw ( 200 );
}

void	Request::handleTransferEncoding()
{
	std::string chunkLine;
	std::string chunkData;

	for (size_t i = 0; i < buffer.size(); i++ )
	{
		chunkLine += buffer.at( i );

		if (chunkLine.find("\r\n") != std::string::npos)
		{
			chunkLine.erase(chunkLine.size() - 2);

			if (chunkIndex == 0)
			{
				chunkLength = ConvertHex(chunkLine);
				chunkIndex++;
			}
			else
			{
				chunkData += chunkLine;
				chunkIndex++;
			}

			if (chunkIndex == 2)
			{
				if (chunkLength != chunkData.length())
					throw ( 4 );
				if (chunkLength == 0)
					throw ( 200 );
				body.insert(body.end(), chunkData.begin(), chunkData.end());
				chunkIndex = 0;
				chunkLine.clear();
				chunkData.clear();
			}
		}
	}
}

void	Request::bodyFill()
{
	if (inBody == true && !buffer.empty())
	{
		if (headerExist( "content-length" ))
			handleContentLength();
		else
			handleTransferEncoding();
	}
}

void	Request::print()
{
	std::cout << " ****************************Request Line***********************************" << std::endl;
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
	::print(body);
}

void Request::clear()
{
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
	fragement.clear();
	Finalpath.clear();
	query.clear();
}

/* ************************************************************************** */
/*				Setup											  			  */
/* ************************************************************************** */

void Request::setup( std::vector<char> & newBuffer )
{
	buffer = newBuffer;
	// ::print(buffer);
	try {
		headerFill();
		bodyFill();
	}
	catch ( int & e )
	{
		// print();
		statusCode = e;
		std::cout << "Status Code " << statusCode << std::endl;
		throw( e );
	};
}
