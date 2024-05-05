#ifndef LOCATIONCONTEXT_HPP
# define LOCATIONCONTEXT_HPP
# include <vector>

class LocationContext {
	private:
		std::vector< std::vector< std::string > > locationDirectives;
	public:
		LocationContext();
		LocationContext( const LocationContext & locationContext );
		LocationContext & operator=( const LocationContext & locationContext );
		~LocationContext();
		std::vector< std::vector< std::string > > get_locationDirectives() const;
		void set_locationDirectives( std::vector< std::string > directive );
		void clear();
		void print();
};

#endif
