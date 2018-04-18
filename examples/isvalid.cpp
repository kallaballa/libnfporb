//#define LIBNFP_USE_RATIONAL
#include "../src/libnfporb.hpp"

int main(int argc, char** argv) {
  using namespace libnfporb;

	polygon_t pA;
	read_wkt_polygon(argv[1], pA);

	if(bg::is_valid(pA))
		return 0;
	else
		return 1;
}

