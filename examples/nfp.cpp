#include "../src/libnfp.hpp"

int main(int argc, char** argv) {
	polygon_t pA;
	polygon_t pB;
	//read polygons from wkt files
	read_wkt_polygon(argv[1], pA);
	read_wkt_polygon(argv[2], pB);

	//generate NFP
	nfp_t nfp = generateNFP(pA, pB);

	//write and svg containing pA, pB and NFP
	write_svg("nfp.svg",{pA,pB},nfp);
	return 0;
}

