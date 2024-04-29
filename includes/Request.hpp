/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndahib <ndahib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 09:28:30 by ndahib            #+#    #+#             */
/*   Updated: 2024/04/28 12:44:05 by ndahib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef REQUEST_HPP
# define REQUEST_HPP 

#include "webserv.hpp"

#define MAX_SIZE_LENGHT 254

class	Request
{
	/* ***********************************Methods************************ */		
	public:
		Request();
		Request(const Request &obj);
		Request &operator=(const Request &obj);
		~Request();
	
	private:
		void	split_request();
		void	parse_request();
		void	parse_request_line();
		bool	HostAlreadyExist();
		void	parse_headers();
		void	parse_body();
		void	parse_path();
		void 	checkChunked();
		void 	handleChunked();
		void	print();
	
	public:
		void				clear();
		void				setup( std::vector<char> & newBuffer );
		bool				headerExist( const std::string & key ) const;
		std::string const	getHeader( const std::string & key ) const;

	/* *******************************Attributes************************ */
	private:
		// General Elements;
		std::vector<char> 			buffer;
		std::vector<std::string>	TmpHeaders;

		// For Chunked
		size_t						chunkLength;
		std::string					chunkBuffer;
		std::pair<std::string, std::string> chunkPair;
		
	public:
		int							statusCode;
		// http Request have 3 component:
			//1- Request Line : that contain Mthod Path and HTTP version;
		std::string			path;
		std::string			method;
		std::string			protocol;
		std::string			requestLine;
			//1.1 Components  of Path
		std::string			fragement;
		std::string			Finalpath;
		std::map<std::string, std::string>  query;
		
			//2- Headres 
		std::vector<std::pair<std::string, std::string> >	header;
		
			//3- Message Body
		std::vector<char>			body;
		std::vector<char>			Finalbody;
};

# endif /*REQUEST_HPP*/
