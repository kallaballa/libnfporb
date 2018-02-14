//#define LIBNFP_USE_RATIONAL
#include "../src/libnfp.hpp"

int main(int argc, char** argv) {
  using namespace libnfp;

	polygon_t pA;
	polygon_t pB;
	//read polygons from wkt files
	read_wkt_polygon(argv[1], pA);
	read_wkt_polygon(argv[2], pB);

  //generate NFP of polygon A and polygon B and check the polygons for validity.
  //When the third parameters is false validity check is skipped for a little performance increase
	nfp_t nfp = generateNFP(pA, pB, true);

	//write and svg containing pA, pB and NFP
	write_svg("nfp.svg",{pA,pB},nfp);
	return 0;
}

