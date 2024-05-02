/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndahib <ndahib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 09:28:30 by ndahib            #+#    #+#             */
/*   Updated: 2024/05/02 10:50:56 by ndahib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef REQUEST_HPP
# define REQUEST_HPP 


#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <exception>

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
		void				print();
		void				parse();
		void				bodyFill();
		void				headerFill();
		void				parse_path();
		void				parseHeader();
		void				parseRequestLine();
		void				checkEndOfHeaders();
		void				handleContentLength();
		void				handleTransferEncoding();
	
	public:
		
		void				clear();
		void				setup( std::vector<char> & newBuffer );
		bool				headerExist( const std::string & key ) const;
		std::string const	getHeader( const std::string & key ) const;
		
		/* ****************************Getters************************ */
		
		std::string 		get_path() const;
		std::string			get_method() const;
		int					get_statusCode() const;
		std::vector< char >	get_body() const;
		

	/* *******************************Attributes************************ */
	private:
		// General Elements;
			int 												index;
			int													statusCode;
			bool												inBody;
			std::string											rem;
			std::vector<char> 									buffer;
			


		// For Chunked
			int 												chunkIndex;
			size_t												chunkLength;
			std::string											chunkBuffer;
			
		
		// http Request have 3 component:
			//1- Request Line : that contain Mthod Path and HTTP version;
			std::string											method;
			std::string											path;
			std::string 										protocol;
			//1.1 Components  of Path
			std::string											fragement;
			std::string											Finalpath;
			std::map<std::string, std::string>					query;
		
			//2- Headres 
			std::vector<std::string> 							lines;
			std::vector<std::pair<std::string, std::string> >	header;
			//3- Message Body
			std::vector<char>									 body;

};

# endif /*REQUEST_HPP*/
