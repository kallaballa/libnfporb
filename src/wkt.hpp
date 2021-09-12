#ifndef SRC_WKT_HPP_
#define SRC_WKT_HPP_

#include "geometry.hpp"

namespace libnfporb {
void read_wkt_polygon(const string& filename, polygon_t& p) {
	std::ifstream t(filename);

	std::string str;
	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());

	str.pop_back();
	bg::read_wkt(str, p);
	bg::correct(p);
}
}


#endif /* SRC_WKT_HPP_ */
