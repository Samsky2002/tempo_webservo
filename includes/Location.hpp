#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <vector>
# include <iostream>

class Location
{
	private:
		std::string path;
		std::vector<std::string> methods;
		std::string redirection;
		std::string root;
		bool autoindex;
		std::vector<std::string> index;
		std::string upload;
	public:
		Location();
		Location(const Location & location);
		Location & operator=(const Location & location);
		~Location();
		std::string get_path() const;
		std::string get_root() const;
		std::string get_redirection() const;
		std::vector< std::string > get_index() const;
		std::string get_index_idx( size_t idx ) const;
		bool get_autoindex() const;
		std::vector< std::string > get_methods() const;
		std::string get_methods_idx( size_t idx ) const;
		std::string get_upload() const;
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
		void check_trailing_slash();
};

#endif
