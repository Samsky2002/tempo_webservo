#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <vector>
# include <iostream>

class Location
{
	public:
		std::string path;
		std::vector<std::string> methods;
		std::string redirection;
		std::string root;
		bool autoindex;
		std::vector<std::string> index;
		std::string upload;
		//std::string cgi;
		Location();
		Location(const Location & location);
		Location & operator=(const Location & location);
		~Location();
		void print();
		void clear();
		void fill( const std::vector< std::vector< std::string > > & directives );
		void fill_location_path( const std::vector< std::string > & directives );
		void fill_methods( const std::vector< std::string > & directive );
		void fill_redirection( const std::vector< std::string > & directive );
		void fill_root( const std::vector< std::string > & directive );
		void fill_autoindex( const std::vector< std::string > & directive );
		void fill_index( const std::vector< std::string > & directive );
		void fill_upload( const std::vector< std::string > & directive );
};

#endif
