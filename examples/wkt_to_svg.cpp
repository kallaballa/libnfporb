#include "../src/libnfporb.hpp"

int main(int argc, char** argv) {
  using namespace libnfporb;

	polygon_t pA;
	read_wkt_polygon(argv[1], pA);
	write_svg(argv[2],{pA});

	return 0;
}
