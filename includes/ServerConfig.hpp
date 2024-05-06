#ifndef SERVERCONFIG_HPP 
# define SERVERCONFIG_HPP
# include <vector>
# include <iostream>
# include "Location.hpp"


class ServerConfig
{
	private:
		std::vector< std::string > server_names;
		std::vector< std::pair<int, std::string> > error_pages;
		int client_max_body_size;
		std::vector<Location> location;
	public:
		ServerConfig();
		ServerConfig( const ServerConfig & serverConf );
		ServerConfig & operator=( const ServerConfig & serverConf );
		~ServerConfig();
		void set_location( Location location );
		std::vector< Location > get_location() const;
		Location get_location_idx( size_t idx ) const;
		std::vector< std::string > get_server_names() const;
		std::string get_server_names_idx( size_t idx ) const;
		std::vector< std::pair<int, std::string > > get_error_pages() const;
		std::pair<int, std::string> get_error_pages_idx( size_t idx ) const;
		void print();
		void clear();
		void fill( const std::vector< std::vector< std::string > > & directives ); 
		void fill_server_name( const std::vector< std::string > & directive );
		void fill_error_page( const std::vector< std::string > & directive );
		void fill_client_max_body_size( const std::vector< std::string > & directive );
};

// we could make error_pages a pair of vector that has the status and path

#endif
