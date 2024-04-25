#ifndef LOCATIONCONTEXT_HPP
# define LOCATIONCONTEXT_HPP
# include <vector>

class LocationContext {
	public:
	std::vector< std::vector< std::string > > locationDirectives;
	void clear();
	void print();
};

#endif
